// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAttributeSet.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraAbilityType.h"
#include "Net/UnrealNetwork.h"
#include"GameFramework/Character.h"
#include "AuraGameplayTags.h"
#include "GameplayAbilitySet.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Interfaction/CombatInterface.h"
#include "Interfaction/PlayerInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerController.h"


UAuraAttributeSet::UAuraAttributeSet()
{
	/**属性访问器初始化属性**/
	//InitHealth(50.f);  直接用GE进行初始化
	//InitMaxHealth(100.f);
	//InitMana(25.f);
	//InitMaxMana(50.f);
	const FAuraGameplayTags& AuraGameplayTags = FAuraGameplayTags::Get();

	//主要属性
	TagToAttributes.Add(AuraGameplayTags.Attributes_Primary_Strength,GetStrengthAttribute);
	TagToAttributes.Add(AuraGameplayTags.Attributes_Primary_Intelligence,GetIntelligenceAttribute);
	TagToAttributes.Add(AuraGameplayTags.Attributes_Primary_Resilience,GetResilienceAttribute);
	TagToAttributes.Add(AuraGameplayTags.Attributes_Primary_Vigor,GetVigorAttribute);

	//次要属性
	TagToAttributes.Add(AuraGameplayTags.Attributes_Secondary_Armor,GetArmorAttribute);
	TagToAttributes.Add(AuraGameplayTags.Attributes_Secondary_ArmorPenetration,GetArmorPenetrationAttribute);
	TagToAttributes.Add(AuraGameplayTags.Attributes_Secondary_BlockChance,GetBlockChanceAttribute);
	TagToAttributes.Add(AuraGameplayTags.Attributes_Secondary_CriticalHitChance,GetCriticalHitChanceAttribute);
	TagToAttributes.Add(AuraGameplayTags.Attributes_Secondary_CriticalHitDamage,GetCriticalHitDamageAttribute);
	TagToAttributes.Add(AuraGameplayTags.Attributes_Secondary_CriticalHitResistance,GetCriticalHitResistanceAttribute);
	TagToAttributes.Add(AuraGameplayTags.Attributes_Secondary_HealthRegeneration,GetHealthRegenerationAttribute);
	TagToAttributes.Add(AuraGameplayTags.Attributes_Secondary_ManaRegeneration,GetManaRegenerationAttribute);
	TagToAttributes.Add(AuraGameplayTags.Attributes_Secondary_MaxHealth,GetMaxHealthAttribute);
	TagToAttributes.Add(AuraGameplayTags.Attributes_Secondary_MaxMana,GetMaxManaAttribute);

	//抗性
	TagToAttributes.Add(AuraGameplayTags.Attributes_Resistance_Arcane,GetArcaneResistanceAttribute);
	TagToAttributes.Add(AuraGameplayTags.Attributes_Resistance_Physical,GetPhysicalResistanceAttribute);
	TagToAttributes.Add(AuraGameplayTags.Attributes_Resistance_Fire,GetFireResistanceAttribute);
	TagToAttributes.Add(AuraGameplayTags.Attributes_Resistance_Lightning,GetLightningResistanceAttribute);
}

void UAuraAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//主要属性
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,Strength,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,Intelligence,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,Resilience,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,Vigor,COND_None,REPNOTIFY_Always);
	//次要属性
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,Armor,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,ArmorPenetration,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,BlockChance,COND_None,REPNOTIFY_Always); 
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,CriticalHitChance,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,CriticalHitDamage,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,CriticalHitResistance,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,HealthRegeneration,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,ManaRegeneration,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,MaxHealth,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,MaxMana,COND_None,REPNOTIFY_Always);
	//重要属性
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,Health,COND_None,REPNOTIFY_Always);//Health在服务器上更改成功并同步到客户端上时触发的回调函数，参数意外无条件，始终进行复制
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,Mana,COND_None,REPNOTIFY_Always);
	//抗性
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,FireResistance,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,LightningResistance,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,PhysicalResistance,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,ArcaneResistance,COND_None,REPNOTIFY_Always);
	
}

void UAuraAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
//此函数会在属性中的当前值被修改之前调用，用于强制实施与属性相关的规则，例如限制某个属性的值为必须为某个区间，但是由于此函数是调用与属性修改之前，而属性修改之后的某些操作例如:
//某些陷阱或者debuff直接修改了属性值,会导致对该值的重新计算，由于修改发生在属性其修改之后，所以用这个函数做的限制就会被打破，即此函数不是用于限制的最佳选择
//为什么会失效呢:GameEffect的作用时间在此函数执行之后，会在此值被限制的基础上再次运算，导致限制失去效果,所以需要在所有运算完成之后再次进行限制
{
	Super::PreAttributeChange(Attribute, NewValue);
	if(Attribute==GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue,0.f,GetMaxHealth());
		//UE_LOG(LogTemp,Warning,TEXT("Health :%f"),NewValue);
	}
	if(Attribute==GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue,0.f,GetMaxMana());
		//UE_LOG(LogTemp,Warning,TEXT("Mana: %f"),NewValue);
	}
}

void UAuraAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)    //此函数为属性值被修改之后再进行的一些操作，用于对属性进行最终的修改
/**这个FGameplayEffectModCallbackData包含了游戏效果的所有的回调信息，包括不限于上下文包含的信息，以及GameplayEffectSpec中的信息，非常之多，可以打个断点运行时慢慢查看**/
{
	Super::PostGameplayEffectExecute(Data);
	FEffectProperties Props;
	SetEffectProperties(Data,Props);
	if(Props.TargetCharacter->Implements<UCombatInterface>() && ICombatInterface::Execute_IsDead(Props.TargetCharacter)) return;
	if(Data.EvaluatedData.Attribute==GetHealthAttribute())
		/**EvaluatedData是一个FGameplayModifierEvaluatedData 类型的结构体，储存了属性在修改时发生的情况,主要成员包括；
	*Attribute：表示被修改的属性。Handle：表示来源于哪个活动的游戏效果。在许多情况下，这个句柄可能无效。
	*IsValid：如果有评估发生，为 true。Magnitude：应用的属性的原始数值，通常是在进行限制之前的数值。ModifierOp：这个修改器的数值操作类型，例如覆盖、添加、乘法等。
		 ***/
	{
		SetHealth(FMath::Clamp(GetHealth(),0.f,GetMaxHealth()));
		UE_LOG(LogTemp, Warning, TEXT("Changed Health on %s, Health: %f"), *Props.TargetAvatarActor->GetName(), GetHealth());
	}
	if(Data.EvaluatedData.Attribute==GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(),0.f,GetMaxMana()));
	}

	if(Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())		//元属性做占位数据用于伤害值
	{
		HandleIncomingDamage(Props);
	}
	if (Data.EvaluatedData.Attribute == GetIncomingXPAttribute())
	{
		HandleIncomingXP(Props);
	}
}

void UAuraAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
	if(Attribute==GetMaxHealthAttribute()&&bTopOffHealth)
	{
		SetHealth(GetMaxHealth());
		bTopOffHealth = false;
	}
	if(Attribute==GetMaxManaAttribute()&&bTopOffMana)
	{
		SetMana(GetMaxMana());
		bTopOffMana = false;
	}
}


void UAuraAttributeSet::HandleIncomingDamage(const FEffectProperties& Props)
{
	const float LocalIncomingDamage = GetIncomingDamage();
	SetIncomingDamage(0.f);
	if(LocalIncomingDamage>0.f)
	{
		const float NewHealth = GetHealth()-LocalIncomingDamage;
		SetHealth(FMath::Clamp(NewHealth,0.f,GetMaxHealth()));
			
		const bool bFatal = NewHealth<=0.f;

		if(bFatal)
		{
			ICombatInterface* CombatInterface = Cast<ICombatInterface>(Props.TargetAvatarActor);
			if(CombatInterface)
			{
				FVector Impulse = UAuraAbilitySystemLibrary::GetDeathImpulse(Props.EffectContextHandle);
				CombatInterface->Die(Impulse);		//死亡时调用
			}
			SendXPEvent(Props);
		}
		else//没死的情况触发受击
		{
			FGameplayTagContainer TagContainer;
			TagContainer.AddTag(FAuraGameplayTags::Get().Effect_HitReact);
			Props.TargetASC->TryActivateAbilitiesByTag(TagContainer);

			const FVector& KnockbackForce = UAuraAbilitySystemLibrary::GetKnockbackForce(Props.EffectContextHandle);
			if (!KnockbackForce.IsNearlyZero(1.f))
			{
				Props.TargetCharacter->LaunchCharacter(KnockbackForce, true, true);
			}
		}
		const bool bBlock = UAuraAbilitySystemLibrary::IsBlocked(Props.EffectContextHandle);
		const bool bCriticalHit = UAuraAbilitySystemLibrary::IsCriticalHit(Props.EffectContextHandle);
			
		ShowFloatingText(Props,LocalIncomingDamage,bBlock,bCriticalHit);		//伤害计算的同时将伤害值传入Aura控制器中的显示函数,再通过调用函数创建出Text组件
		if(UAuraAbilitySystemLibrary::IsSuccessfulDebuff(Props.EffectContextHandle))	//计算伤害时设置Spec中的debuff相关的信息变量。
		{
			Debuff(Props);		//判定成功施加debuff时施加debuff
		}
	}
}

void UAuraAttributeSet::HandleIncomingXP(const FEffectProperties& Props)
{
	const float LocalIncomingXP = GetIncomingXP();		//此次击杀获得的经验
	SetIncomingXP(0.f);

	if(Props.SourceCharacter->Implements<UPlayerInterface>())
	{
		const int32 CurrentLevel = ICombatInterface::Execute_GetPlayerLevel(Props.SourceCharacter);	//获取当前等级
		const int32 CurrentXP = IPlayerInterface::Execute_GetXP(Props.SourceCharacter);		//获取当前经验

		const int32 NewLevel = IPlayerInterface::Execute_FindLevelForXP(Props.SourceCharacter, CurrentXP + LocalIncomingXP);	//查找当前经验加上获得的经验可以达到的等级
		const int32 NumLevelUps = NewLevel - CurrentLevel;		//提升的等级
		if (NumLevelUps > 0)
		{
			const int32 AttributePointsReward = IPlayerInterface::Execute_GetAttributePointsReward(Props.SourceCharacter, CurrentLevel);
			const int32 SpellPointsReward = IPlayerInterface::Execute_GetSpellPointsReward(Props.SourceCharacter, CurrentLevel);

			IPlayerInterface::Execute_AddToPlayerLevel(Props.SourceCharacter, NumLevelUps);
			IPlayerInterface::Execute_AddToAttributePoints(Props.SourceCharacter, AttributePointsReward);
			IPlayerInterface::Execute_AddToSpellPoints(Props.SourceCharacter, SpellPointsReward);

			//SetHealth(GetMaxHealth());		//升级重置生命和魔法值
			//SetMana(GetMaxMana());		升级之后最大值会增加，而增加的时机在此函数之后，就会导致设置不到正确的值，重写PostAttributeChange并在里面设置
			bTopOffHealth = true;
			bTopOffMana = true;
			IPlayerInterface::Execute_LevelUp(Props.SourceCharacter);
		}
		IPlayerInterface::Execute_AddToXP(Props.SourceCharacter,LocalIncomingXP);	//调用接口函数增加经验
	}
}

void UAuraAttributeSet::Debuff(const FEffectProperties& Props)
{
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	FGameplayEffectContextHandle EffectContext = Props.SourceASC->MakeEffectContext();		//创建上下文
	EffectContext.AddSourceObject(Props.SourceAvatarActor);		//添加源Actor

	const FGameplayTag DamageType = UAuraAbilitySystemLibrary::GetDamageType(Props.EffectContextHandle);	//分别获取伤害类型，debuff伤害，持续时间和频率
	const float DebuffDamage = UAuraAbilitySystemLibrary::GetDebuffDamage(Props.EffectContextHandle);
	const float DebuffDuration = UAuraAbilitySystemLibrary::GetDebuffDuration(Props.EffectContextHandle);
	const float DebuffFrequency = UAuraAbilitySystemLibrary::GetDebuffFrequency(Props.EffectContextHandle);

	FString DebuffName = FString::Printf(TEXT("DynamicDebuff_%s"), *DamageType.ToString());
	UGameplayEffect* Effect = NewObject<UGameplayEffect>(GetTransientPackage(), FName(DebuffName));		//创建GE

	Effect->DurationPolicy = EGameplayEffectDurationType::HasDuration;		//设置debuff为持续类型
	Effect->Period = DebuffFrequency;		//设置周期，即频率
	Effect->DurationMagnitude = FScalableFloat(DebuffDuration);		//设置持续时间

	Effect->CachedGrantedTags.AddTag(GameplayTags.DamageTypesToDebuffs[DamageType]);	//设置伤害类型对应的debuff类型Tag,注意5.3由于变动原来的InheritableOwnedTagsContainer已弃用。

	Effect->StackingType = EGameplayEffectStackingType::AggregateBySource;		//设置栈叠加的方式
	Effect->StackLimitCount = 1;		//设置栈数量限制

	const int32 Index = Effect->Modifiers.Num();
	Effect->Modifiers.Add(FGameplayModifierInfo());
	FGameplayModifierInfo& ModifierInfo = Effect->Modifiers[Index];

	ModifierInfo.ModifierMagnitude = FScalableFloat(DebuffDamage);
	ModifierInfo.ModifierOp = EGameplayModOp::Additive;
	ModifierInfo.Attribute = UAuraAttributeSet::GetIncomingDamageAttribute();

	if (FGameplayEffectSpec* MutableSpec = new FGameplayEffectSpec(Effect, EffectContext, 1.f))	//通过设置好的Effect创建Spec
	{
		FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(MutableSpec->GetContext().Get());
		TSharedPtr<FGameplayTag> DebuffDamageType = MakeShareable(new FGameplayTag(DamageType));
		AuraContext->SetDamageType(DebuffDamageType);

		Props.TargetASC->ApplyGameplayEffectSpecToSelf(*MutableSpec);	//应用debuff	
	}
}

void UAuraAttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const
{

	//Source为GameplayEffect的来源，Target为GameplayEffect的目标，即ASC的拥有者;此函数通过Data将效果的信息储存在FEffectProperties中便于后续使用
	Props.EffectContextHandle = Data.EffectSpec.GetEffectContext();
	Props.SourceASC = Props.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();
	/**为什么来源会有Controller和Character呢，虽然还没有做到，但是可以大胆猜测一下:虽然现阶段只有EffectActor这类回复生命值和魔法值的Effect，但是以后会有Ai敌人所造成的伤害，此时伤害来源就有这两个东西了**/
	if(IsValid(Props.SourceASC)&&Props.SourceASC->AbilityActorInfo.IsValid()&&Props.SourceASC->AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.SourceController = Props.SourceASC->AbilityActorInfo->PlayerController.Get();
		Props.SourceAvatarActor = Props.SourceASC->AbilityActorInfo->AvatarActor.Get();  //这里获取的AvatarActor是一个Actor类型的弱指针，由于Effect的来源肯定是一个Actor,所以正常情况下不会为空
		if(Props.SourceController==nullptr&&Props.SourceAvatarActor!=nullptr)
		{
			if(const APawn* Pawn = Cast<APawn>(Props.SourceAvatarActor))
				Props.SourceController = Pawn->GetController();
		}
		if(Props.SourceController)
		{
			Props.SourceCharacter = Cast<ACharacter>(Props.SourceController->GetPawn());
		}
	}
	if(Data.Target.AbilityActorInfo.IsValid()&&Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		Props.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		Props.TargetCharacter = Cast<ACharacter>(Props.TargetAvatarActor);  //这里一开始写的时候把参数写成了Props.TargetController->GetPawn(),导致初始化ai的时候目标实际上也是角色，一运行就会崩溃。
		Props.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Props.TargetAvatarActor);
		//TargetASC赋值这里查了一下为什么不能用Data.Target.AbilityActorInfo->AbilitySystemComponent.Get(),得到的回答是能力组件不一定存在，这种获取方式不安全，不过是否正确不是很确定
	}
}

void UAuraAttributeSet::ShowFloatingText(const FEffectProperties& Props, float Damage,bool bBlocked,bool bCriticalHit) const
{
	if(Props.SourceCharacter!=Props.TargetCharacter)
	{
		if(AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(Props.SourceCharacter->Controller))//获取玩家的控制并转换
		{
			AuraPlayerController->ShowDamageText(Damage,Props.TargetCharacter,bBlocked,bCriticalHit);
			return;
		}
		if(AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(Props.TargetCharacter->Controller))//通过AI控制器触发
		{
			AuraPlayerController->ShowDamageText(Damage,Props.TargetCharacter,bBlocked,bCriticalHit);
		}
	}
}

void UAuraAttributeSet::SendXPEvent(const FEffectProperties& Props)
{
	if(Props.TargetCharacter->Implements<UCombatInterface>())
	{
		const int32 TargetLevel = ICombatInterface::Execute_GetPlayerLevel(Props.TargetCharacter);	//获取敌人等级
		const ECharacterClass CharacterClass = ICombatInterface::Execute_GetCharacterClass(Props.TargetCharacter);//获取敌人类型
		const int32 XPReward = UAuraAbilitySystemLibrary::GetXPRewardForClassAndLevel(Props.TargetCharacter,CharacterClass,TargetLevel);	//获取该敌人掉落的经验

		const FAuraGameplayTags& GameplayTag = FAuraGameplayTags::Get();
		FGameplayEventData Payload;
		Payload.EventTag = GameplayTag.Attributes_Meta_IncomingXP;
		Payload.EventMagnitude = XPReward;

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Props.SourceCharacter
			,GameplayTag.Attributes_Meta_IncomingXP,Payload);		//敌人死亡的时候调用该函数触发事件
	}
}


//主要属性**********************************
void UAuraAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldStrength) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,Strength,OldStrength);
}

void UAuraAttributeSet::OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const 
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,Intelligence,OldIntelligence);
}

void UAuraAttributeSet::OnRep_Resilience(const FGameplayAttributeData& OldResilience) const 
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,Resilience,OldResilience);
}

void UAuraAttributeSet::OnRep_Vigor(const FGameplayAttributeData& OldVigor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,Vigor,OldVigor);
}


//次要属性*************************************************
void UAuraAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,Armor,OldArmor);
}

void UAuraAttributeSet::OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,ArmorPenetration,OldArmorPenetration);
}

void UAuraAttributeSet::OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,BlockChance,OldBlockChance);
}

void UAuraAttributeSet::OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,CriticalHitChance,OldCriticalHitChance);
}

void UAuraAttributeSet::OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,CriticalHitDamage,OldCriticalHitDamage);
}

void UAuraAttributeSet::OnRep_CriticalHitResistance(const FGameplayAttributeData& OldCriticalHitResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,CriticalHitResistance,OldCriticalHitResistance);
}

void UAuraAttributeSet::OnRep_HealthRegeneration(const FGameplayAttributeData& OldHealthRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,HealthRegeneration,OldHealthRegeneration);
}

void UAuraAttributeSet::OnRep_ManaRegeneration(const FGameplayAttributeData& OldManaRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,ManaRegeneration,OldManaRegeneration);
}

void UAuraAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,MaxHealth,OldMaxHealth);
}


void UAuraAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,MaxMana,OldMaxMana);
}


//重要属性*********************************************
void UAuraAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const  //当客户端接收到Health更新的消息会自动调用
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,Health,OldHealth);   //回调处理Health的变化
}

void UAuraAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,Mana,OldMana);
}


//抗性*************************************************************
void UAuraAttributeSet::OnRep_FireResistance(const FGameplayAttributeData& OldFireResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,FireResistance,OldFireResistance);
}

void UAuraAttributeSet::OnRep_LightningResistance(const FGameplayAttributeData& OldLightningResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,LightningResistance,OldLightningResistance);
}

void UAuraAttributeSet::OnRep_PhysicsResistance(const FGameplayAttributeData& OldPhysicsResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,PhysicalResistance,OldPhysicsResistance);
}

void UAuraAttributeSet::OnRep_ArcaneResistance(const FGameplayAttributeData& OldArcaneResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,ArcaneResistance,OldArcaneResistance);
}

