// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ExecCal_Damage.h"

#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include  "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Interfaction/CombatInterface.h"

struct AuraDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);				//属性捕获
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage);
	
	DECLARE_ATTRIBUTE_CAPTUREDEF(FireResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(LightningResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArcaneResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalResistance);

	TMap<FGameplayTag,FGameplayEffectAttributeCaptureDefinition>TagsToCaptureDefs;	//FGameplayEffectAttributeCaptureDefinition定义了属性捕获的选项：源，要捕获的属性，是否使用快照,捕获属性的Def就是这个类型的结构体
	
	AuraDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet,Armor,Target,false);		//不使用快照捕获
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet,BlockChance,Target,false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet,ArmorPenetration,Source,false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet,CriticalHitChance,Source,false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet,CriticalHitResistance,Target,false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet,CriticalHitDamage,Source,false);

		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, FireResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, LightningResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArcaneResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, PhysicalResistance, Target, false);

		const FAuraGameplayTags& Tags = FAuraGameplayTags::Get();
		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_Armor,ArmorDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_BlockChance,BlockChanceDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_ArmorPenetration,ArmorPenetrationDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_CriticalHitChance,CriticalHitChanceDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_CriticalHitResistance,CriticalHitChanceDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_CriticalHitDamage,CriticalHitDamageDef);

		TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Arcane,ArcaneResistanceDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Fire, FireResistanceDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Lightning, LightningResistanceDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Physical, PhysicalResistanceDef);
	}
};

static const AuraDamageStatics& DamageStatics()		//获取捕获到的属性
{
	static AuraDamageStatics DStatics;
	return DStatics;
}

UExecCal_Damage::UExecCal_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);		//FGameplayEffectAttributeCaptureDefinition类型的数组，储存捕获到的属性
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);			//敌人格挡几率
	RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);			//玩家护甲穿透
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitChanceDef);				//玩家暴击率
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitResistanceDef);		//敌人暴击抗性
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitDamageDef);			//玩家暴击伤害
	
	RelevantAttributesToCapture.Add(DamageStatics().FireResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().LightningResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArcaneResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().PhysicalResistanceDef);
}

void UExecCal_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                             FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;
	//ICombatInterface* SourceCombatInterface = Cast<ICombatInterface>(SourceAvatar);	//获取接口以获得角色等级
	//ICombatInterface* TargetCombatInterface = Cast<ICombatInterface>(TargetAvatar);
	int32 SourcePlayerLevel = 1;
	if (SourceAvatar->Implements<UCombatInterface>())
	{
		SourcePlayerLevel = ICombatInterface::Execute_GetPlayerLevel(SourceAvatar);
	}
	int32 TargetPlayerLevel = 1;
	if (TargetAvatar->Implements<UCombatInterface>())
	{
		TargetPlayerLevel = ICombatInterface::Execute_GetPlayerLevel(TargetAvatar);
	}

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();	//GetAggregatedTags返回一个包含Spec及TagContainer的结构体
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	// float Armor = 0.f;
	// ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluationParameters, Armor);	//Def储存的捕获信息，例如属性名称，是否捕获源等
	// Armor = FMath::Max<float>(0.f,Armor);		//更新Armor之后确保值大于0

	//float Damage = Spec.GetSetByCallerMagnitude(FAuraGameplayTags::Get().Damage);	//获取伤害
	float Damage = 0.f;
	for(const TTuple<FGameplayTag,FGameplayTag>&Pair : FAuraGameplayTags::Get().DamageTypesToResistances)		//TTuple是虚幻引擎中的元组,与TPair的区别可以简单理解为元组可以储存两个以上的值
	{
		const FGameplayTag DamageType = Pair.Key;
		const FGameplayTag ResistanceTag = Pair.Value;		//储存了每种伤害类型Tag对应的抗性Tag

		checkf(AuraDamageStatics().TagsToCaptureDefs.Contains(ResistanceTag),
			TEXT("TagsToCaptureDefs doesn't contain Tag: [%s] in ExecCalc_Damage"), *ResistanceTag.ToString());

		const FGameplayEffectAttributeCaptureDefinition CaptureDef = AuraDamageStatics().TagsToCaptureDefs[ResistanceTag];	//通过抗性的Tag获取捕获属性
		float DamageTypeValue = Spec.GetSetByCallerMagnitude(Pair.Key,false);		//基础伤害值,不加false会导致没查到会输出一个log
		float Resistance = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CaptureDef,EvaluationParameters,Resistance);	//获取属性对应的抗性
		Resistance = FMath::Max<float>(Resistance,0.f);

		DamageTypeValue *= ( 100.f - Resistance ) / 100.f;		//将对应的属性抗性加入伤害计算
		Damage+= DamageTypeValue;
	}

	float TargetBlockChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef,EvaluationParameters,TargetBlockChance);	//获取格挡几率
	TargetBlockChance = FMath::Max<float>(TargetBlockChance, 0.f);
	
	const bool bBlock = FMath::RandRange(1,100)<TargetBlockChance;		//判断是否格挡

	FGameplayEffectContextHandle ContextHandle = Spec.GetContext();			//设置Context是否被格挡
	UAuraAbilitySystemLibrary::SetIsBlocked(ContextHandle,bBlock);
	
	Damage = bBlock ? Damage/2.f : Damage;		//计算格挡后的伤害

	float TargetArmor = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef,EvaluationParameters,TargetArmor);
	TargetArmor = FMath::Max<float>(TargetArmor,0.f);		//获取目标的护甲值并确保大于0

	float SourceArmorPenetration = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationDef,EvaluationParameters,SourceArmorPenetration);
	SourceArmorPenetration = FMath::Max<float>(SourceArmorPenetration,0.f);		//获取角色的护甲穿透并确保大于0

	const UCharacterClassInfo* CharacterClassInfo = UAuraAbilitySystemLibrary::GetCharacterInfo(SourceAvatar);		//获取角色信息

	const FRealCurve* ArmorPenetrationCurve = CharacterClassInfo->DamageCalculationCoefficient->FindCurve(FName("ArmorPenetration"),FString());
	const float ArmorPenetrationCoefficient = ArmorPenetrationCurve->Eval(SourcePlayerLevel);		//从护甲穿透曲线中获取该等级对应的值
	const float EffectiveArmor = TargetArmor * ( 100 - SourceArmorPenetration * ArmorPenetrationCoefficient ) / 100.f;	//忽略一部分护甲
	
	const FRealCurve* EffectiveArmorCurve = CharacterClassInfo->DamageCalculationCoefficient->FindCurve(FName("EffectiveArmor"),FString());
	const float EffectiveArmorCoefficient = EffectiveArmorCurve->Eval(TargetPlayerLevel);		//从护甲曲线图中获取该等级对应的值
	
	Damage *= ( 100 - EffectiveArmor * EffectiveArmorCoefficient ) / 100.f; 	//还生效的护甲忽略一部分伤害

	float SourceCriticalHitChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitChanceDef,EvaluationParameters,SourceCriticalHitChance);
	SourceCriticalHitChance = FMath::Max<float>(SourceCriticalHitChance,0.f);
	
	float TargetCriticalHitResistance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitResistanceDef,EvaluationParameters,TargetCriticalHitResistance);
	TargetCriticalHitResistance = FMath::Max<float>(TargetCriticalHitResistance,0.f);

	float SourceCriticalHitDamage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitDamageDef,EvaluationParameters,SourceCriticalHitDamage);
	SourceCriticalHitDamage = FMath::Max<float>(SourceCriticalHitDamage,0.f);

	const FRealCurve* CriticalHitResistanceCurve = CharacterClassInfo->DamageCalculationCoefficient->FindCurve(FName("CriticalHitResistance"),FString());
	const float CriticalHitResistanceCoefficient = CriticalHitResistanceCurve->Eval(TargetPlayerLevel);

	const float EffectiveCriticalHitChance = SourceCriticalHitChance - TargetCriticalHitResistance * CriticalHitResistanceCoefficient;
	const bool bCriticalHit = FMath::RandRange(1, 100) < EffectiveCriticalHitChance;

	UAuraAbilitySystemLibrary::SetIsCriticalHit(ContextHandle,bCriticalHit);

	Damage = bCriticalHit ? 2.f * Damage + SourceCriticalHitDamage : Damage;
	
	const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);	//Property储存的是捕获的属性本身，枚举为修改器的计算方式,将Damage的值赋给传入的属性
	OutExecutionOutput.AddOutputModifier(EvaluatedData);		//添加到执行输出以供后续在GE中用于计算
}
