// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/Data/LevelInfo.h"
#include "Player/AuraPlayerState.h"

void UOverlayWidgetController::BroadcastInitialValue()   //初始化的时候被调用，广播属性初始值
{
	//const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributesSet);
	//auraWidgetcontroller中所有属性相关的变量都是基类类型，需要先类型转换;基础信息储存为基类中的私有变量了，需要通过函数获取
	OnHealthChanged.Broadcast(GetAuraAS()->GetHealth());
	OnMaxHealthChanged.Broadcast(GetAuraAS()->GetMaxHealth());
	OnManaChanged.Broadcast(GetAuraAS()->GetMana());
	OnMaxManaChanged.Broadcast(GetAuraAS()->GetMaxMana());
}

void UOverlayWidgetController::BindCallbacksDependencies()   //绑定属性变化的回调,绑定的时机为AuraHUD中对OverlayWidgetController初始完成之后
{
	//AAuraPlayerState * AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState);
	GetAuraPS()->OnXPChangedDelegate.AddUObject(this,&UOverlayWidgetController::OnXPChanged);	//绑定函数到玩家状态中的委托，
	GetAuraPS()->OnLevelChangedDelegate.AddLambda(		//当玩家等级变化时触发Lambda表达式，将新的等级广播到控制器中
		[this](int32 NewLevel)
		{
			OnPlayerLevelChangedDelegate.Broadcast(NewLevel);
		});
	//const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributesSet);
	//***************************绑定改为Lambda表达式
	// AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
	// 	AuraAttributeSet->GetHealthAttribute()).AddUObject(this,&UOverlayWidgetController::HealthChanged);  //注意之前宏定义的属性访问器，GetHealth()返回的是一个浮点数，而这里这个函数返回的是一个属性
	// AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
	// 	AuraAttributeSet->GetMaxHealthAttribute()).AddUObject(this,&UOverlayWidgetController::MaxHealthChanged);
	// AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
	// 	AuraAttributeSet->GetManaAttribute()).AddUObject(this,&UOverlayWidgetController::ManaChanged);
	// AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
	// 	AuraAttributeSet->GetMaxManaAttribute()).AddUObject(this,&UOverlayWidgetController::MaxManaChanged);
	//*************************************************************
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		GetAuraAS()->GetHealthAttribute()).AddLambda([this](const FOnAttributeChangeData& Data){ OnHealthChanged.Broadcast(Data.NewValue); });
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		GetAuraAS()->GetMaxHealthAttribute()).AddLambda([this](const FOnAttributeChangeData& Data){ OnMaxHealthChanged.Broadcast(Data.NewValue); });
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		GetAuraAS()->GetManaAttribute()).AddLambda([this](const FOnAttributeChangeData& Data){ OnManaChanged.Broadcast(Data.NewValue); });
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		GetAuraAS()->GetMaxManaAttribute()).AddLambda([this](const FOnAttributeChangeData& Data){ OnMaxManaChanged.Broadcast(Data.NewValue); });

	if(GetAuraASC())
	{
		GetAuraASC()->AbilityEquipped.AddUObject(this,&UOverlayWidgetController::OnAbilityEquipped);
		if(GetAuraASC()->bStartupAbilitiesGiven)
		{
			//OnInitializeStartupAbilities(AuraASC);
			BroadcastAbilityInfo();//技能都初始化完成之后调用函数初始化技能图标相关的信息
		}
		else
		{
			GetAuraASC()->AbilitiesGivenDelegate.AddUObject(this,&UOverlayWidgetController::BroadcastAbilityInfo);//运行到这技能还未初始化完成时将图标初始化函数绑定到技能初始化完成的委托,广播后触发回调。
		}
		GetAuraASC()->EffectAssetTags.AddLambda(   //牢记AuraWidgetController中有包括能力组件，属性集，玩家控制器和玩家状态用于角色间的通信
		[this](const FGameplayTagContainer& AssetTags)  //绑定到Lambda表达式，即匿名函数，是c++11的语法，详情可以找资料看看;如果要调用成员函数，可以捕获this指针，也可以将成员函数设置为静态
		{
			for(const FGameplayTag& tag : AssetTags)   //目前的作用:位于GE中的tag会被添加到容器中，然后通过标签获取数据表中的行，然后打印出相应的信息
			{
				FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag("Message");  //获取一个与Message对应的标签，在项目设置中定义了此标签，用于查找指定属性，如果找不到则返回空的FGameplayTag
				if(tag.MatchesTag(MessageTag))   //检查当前标签是否与MessageTag匹配
				{
					const FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable,tag);
					MessageWidgetRowDelegate.Broadcast(*Row);
				}
			}
		}
		);
	}
}

// void UOverlayWidgetController::OnInitializeStartupAbilities(UAuraAbilitySystemComponent* AuraAbilitySystemComponent)		此函数移动至AuraWidgetController中的BroadcastAbilityInfo
// {
// 	if(!AuraAbilitySystemComponent->bStartupAbilities) return;
// 	//每个技能初始化时创建一个委托并绑定表达式，在能力组件类中调用ForEachAbility触发回调
// 	FForEachAbility BroadcastDelegate;		//传入一个AbilitySpec&类型的参数
// 	BroadcastDelegate.BindLambda([this,AuraAbilitySystemComponent](const FGameplayAbilitySpec& AbilitySpec)
// 	{
// 		FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoByTag(AuraAbilitySystemComponent->GetAbilityTagFromSpec(AbilitySpec));
// 		Info.InputTag = AuraAbilitySystemComponent->GetInputTagFromSpec(AbilitySpec);	//设置两个Tag
// 		Info.AbilityTag = AuraAbilitySystemComponent->GetAbilityTagFromSpec(AbilitySpec);
// 		AbilityInfoDelegate.Broadcast(Info);	//广播信息
// 	});
// 	AuraAbilitySystemComponent->ForEachAbility(BroadcastDelegate);
// }

void UOverlayWidgetController::OnXPChanged(int32 NewXP)		//计算百分比
{
	//const AAuraPlayerState*AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState);
	const ULevelInfo* LevelUpInfo = GetAuraPS()->LevelUpInfo;	//通过玩家状态获取存有升级信息的数字资产
	checkf(LevelUpInfo, TEXT("Unabled to find LevelUpInfo. Please fill out AuraPlayerState Blueprint"));
	const int32 Level = LevelUpInfo->FindLevelForXP(NewXP);	//获取当前等级
	const int32 MaxLevel = LevelUpInfo->LevelUpInformation.Num();
	if(Level<MaxLevel&&Level>0)
	{
		const int32 LevelUpRequirement = LevelUpInfo->LevelUpInformation[Level].LevelRequirement;	//获取当前升级所需经验
		const int32 PreLevelUpRequirement = LevelUpInfo->LevelUpInformation[Level-1].LevelRequirement;	//获取前一级升级所需经验
		const int32 DeltaLevelRequirement = LevelUpRequirement - PreLevelUpRequirement;	//获取从当前等级升级到下一级所需的经验
		const int32 XPForThisLevel = NewXP - PreLevelUpRequirement;	//获取当前经验条填充了多少经验

		const float XPBarPercent = static_cast<float>(XPForThisLevel)/static_cast<float>(DeltaLevelRequirement);	//计算百分比

		OnXPPercentChangedDelegate.Broadcast(XPBarPercent);	//将数据广播到控制器中
	}
}

void UOverlayWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& Status,	//用AbilityTag对应的技能替换掉原有的技能
	const FGameplayTag& Slot, const FGameplayTag& PreviousSlot) const
{
	const FAuraGameplayTags& GameplayTag = FAuraGameplayTags::Get();

	FAuraAbilityInfo LastSlotInfo;		//移除上一个插槽的信息
	LastSlotInfo.StatusTag = GameplayTag.Abilities_Status_Unlocked;
	LastSlotInfo.InputTag = PreviousSlot;
	LastSlotInfo.AbilityTag = GameplayTag.Abilities_None;
	AbilityInfoDelegate.Broadcast(LastSlotInfo);//广播到技能栏
	
	FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoByTag(AbilityTag);	//装备到技能栏的技能。
	Info.StatusTag = Status;
	Info.InputTag = Slot;
	AbilityInfoDelegate.Broadcast(Info);
}

// void UOverlayWidgetController::HealthChanged(const FOnAttributeChangeData& Data) const  
// {
// 	OnHealthChanged.Broadcast(Data.NewValue);
// }
//
// void UOverlayWidgetController::MaxHealthChanged(const FOnAttributeChangeData& Data) const
// {
// 	OnMaxHealthChanged.Broadcast(Data.NewValue);
// }
//
// void UOverlayWidgetController::ManaChanged(const FOnAttributeChangeData& Data) const
// {
// 	OnManaChanged.Broadcast(Data.NewValue);
// }
//
// void UOverlayWidgetController::MaxManaChanged(const FOnAttributeChangeData& Data) const
// {
// 	OnMaxManaChanged.Broadcast(Data.NewValue);
// }
