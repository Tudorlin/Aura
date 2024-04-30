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
	AuraDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet,Armor,Target,false);		//不使用快照捕获
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet,BlockChance,Target,false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet,ArmorPenetration,Source,false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet,CriticalHitChance,Source,false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet,CriticalHitResistance,Target,false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet,CriticalHitDamage,Source,false);
		
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
}

void UExecCal_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                             FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;
	ICombatInterface* SourceCombatInterface = Cast<ICombatInterface>(SourceAvatar);	//获取接口以获得角色等级
	ICombatInterface* TargetCombatInterface = Cast<ICombatInterface>(TargetAvatar);

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();	//GetAggregatedTags返回一个包含Spec及TagContainer的结构体
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	// float Armor = 0.f;
	// ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluationParameters, Armor);	//Def储存的捕获信息，例如属性名称，是否捕获源等
	// Armor = FMath::Max<float>(0.f,Armor);		//更新Armor之后确保值大于0

	float Damage = Spec.GetSetByCallerMagnitude(FAuraGameplayTags::Get().Damage);	//获取伤害

	float TargetBlockChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef,EvaluationParameters,TargetBlockChance);	//获取格挡几率
	TargetBlockChance = FMath::Max<float>(TargetBlockChance, 0.f);
	
	const bool bBlock = FMath::RandRange(1,100)<TargetBlockChance;		//判断是否格挡
	Damage = bBlock ? Damage/2.f : Damage;		//计算格挡后的伤害

	float TargetArmor = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef,EvaluationParameters,TargetArmor);
	TargetArmor = FMath::Max<float>(TargetArmor,0.f);		//获取目标的护甲值并确保大于0

	float SourceArmorPenetration = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationDef,EvaluationParameters,SourceArmorPenetration);
	SourceArmorPenetration = FMath::Max<float>(SourceArmorPenetration,0.f);		//获取角色的护甲穿透并确保大于0

	const UCharacterClassInfo* CharacterClassInfo = UAuraAbilitySystemLibrary::GetCharacterInfo(SourceAvatar);		//获取角色信息

	const FRealCurve* ArmorPenetrationCurve = CharacterClassInfo->DamageCalculationCoefficient->FindCurve(FName("ArmorPenetration"),FString());
	const float ArmorPenetrationCoefficient = ArmorPenetrationCurve->Eval(SourceCombatInterface->GetPlayerLevel());		//从护甲穿透曲线中获取该等级对应的值
	const float EffectiveArmor = TargetArmor * ( 100 - SourceArmorPenetration * ArmorPenetrationCoefficient ) / 100.f;	//忽略一部分护甲
	
	const FRealCurve* EffectiveArmorCurve = CharacterClassInfo->DamageCalculationCoefficient->FindCurve(FName("EffectiveArmor"),FString());
	const float EffectiveArmorCoefficient = EffectiveArmorCurve->Eval(TargetCombatInterface->GetPlayerLevel());		//从护甲曲线图中获取该等级对应的值
	
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
	const float CriticalHitResistanceCoefficient = CriticalHitResistanceCurve->Eval(TargetCombatInterface->GetPlayerLevel());

	const float EffectiveCriticalHitChance = SourceCriticalHitChance - TargetCriticalHitResistance * CriticalHitResistanceCoefficient;
	const bool bCriticalHit = FMath::RandRange(1, 100) < EffectiveCriticalHitChance;

	Damage = bCriticalHit ? 2.f * Damage + SourceCriticalHitDamage : Damage;
	
	const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);	//Property储存的是捕获的属性本身，枚举为修改器的计算方式,将Damage的值赋给传入的属性
	OutExecutionOutput.AddOutputModifier(EvaluatedData);		//添加到执行输出以供后续在GE中用于计算
}
