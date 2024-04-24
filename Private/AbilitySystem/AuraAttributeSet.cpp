// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAttributeSet.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Net/UnrealNetwork.h"
#include"GameFramework/Character.h"
#include "AuraGameplayTags.h"


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

