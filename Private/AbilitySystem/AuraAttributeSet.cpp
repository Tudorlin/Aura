// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAttributeSet.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Net/UnrealNetwork.h"
#include"GameFramework/Character.h"

UAuraAttributeSet::UAuraAttributeSet()
{
	/**属性访问器初始化属性**/
	InitHealth(50.f);
	InitMaxHealth(100.f);
	InitMana(25.f);
	InitMaxMana(50.f);
}

void UAuraAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,Health,COND_None,REPNOTIFY_Always);//Health在服务器上更改成功并同步到客户端上时触发的回调函数，参数意外无条件，始终进行复制
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,MaxHealth,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,Mana,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,MaxMana,COND_None,REPNOTIFY_Always);
}

void UAuraAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
//此函数会在属性中的当前值被修改之前调用，用于强制实施与属性相关的规则，例如限制某个属性的值为必须为某个区间，但是由于此函数是调用与属性修改之前，而属性修改之后的某些操作例如:
//某些陷阱或者debuff直接修改了属性值,会导致对该值的重新计算，由于修改发生在属性其修改之后，所以用这个函数做的限制就会被打破，即此函数不是用于限制的最佳选择
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
		Props.TargetCharacter = Cast<ACharacter>(Props.TargetController->GetPawn());
		Props.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Props.TargetAvatarActor);
		//TargetASC赋值这里查了一下为什么不能用Data.Target.AbilityActorInfo->AbilitySystemComponent.Get(),得到的回答是能力组件不一定存在，这种获取方式不安全，不过是否正确不是很确定
	}
}

void UAuraAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const  //当客户端接收到Health更新的消息会自动调用
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,Health,OldHealth);   //回调处理Health的变化
}

void UAuraAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,MaxHealth,OldMaxHealth);
}

void UAuraAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,Mana,OldMana);
}

void UAuraAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,MaxMana,OldMaxMana);
}
