// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"

void UOverlayWidgetController::BroadcastInitialValue()   //初始化的时候被调用，广播属性初始值
{
	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributesSet);//auraWidgetcontroller中所有属性相关的变量都是基类类型，需要先类型转换
	OnHealthChanged.Broadcast(AuraAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(AuraAttributeSet->GetMaxHealth());
	OnManaChanged.Broadcast(AuraAttributeSet->GetMana());
	OnMaxManaChanged.Broadcast(AuraAttributeSet->GetMaxMana());
}

void UOverlayWidgetController::BindCallbacksDependencies()   //绑定属性变化的回调,绑定的时机为AuraHUD中对OverlayWidgetController初始完成之后
{
	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributesSet);
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
		AuraAttributeSet->GetHealthAttribute()).AddLambda([this](const FOnAttributeChangeData& Data){ OnHealthChanged.Broadcast(Data.NewValue); });
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		AuraAttributeSet->GetMaxHealthAttribute()).AddLambda([this](const FOnAttributeChangeData& Data){ OnMaxHealthChanged.Broadcast(Data.NewValue); });
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		AuraAttributeSet->GetManaAttribute()).AddLambda([this](const FOnAttributeChangeData& Data){ OnManaChanged.Broadcast(Data.NewValue); });
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		AuraAttributeSet->GetMaxManaAttribute()).AddLambda([this](const FOnAttributeChangeData& Data){ OnMaxManaChanged.Broadcast(Data.NewValue); });

	if(UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
	{
		if(AuraASC->bStartupAbilities)
		{
			OnInitializeStartupAbilities(AuraASC);	//技能都初始化完成之后调用函数初始化技能图标相关的信息
		}
		else
		{
			AuraASC->AbilitiesGivenDelegate.AddUObject(this,&UOverlayWidgetController::OnInitializeStartupAbilities);//运行到这技能还未初始化完成时将图标初始化函数绑定到技能初始化完成的委托,广播后触发回调。
		}
		AuraASC->EffectAssetTags.AddLambda(   //牢记AuraWidgetController中有包括能力组件，属性集，玩家控制器和玩家状态用于角色间的通信
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

void UOverlayWidgetController::OnInitializeStartupAbilities(UAuraAbilitySystemComponent* AuraAbilitySystemComponent)
{
	if(!AuraAbilitySystemComponent->bStartupAbilities) return;
	//每个技能初始化时创建一个委托并绑定表达式，在能力组件类中调用ForEachAbility触发回调
	FForEachAbility BroadcastDelegate;		//传入一个AbilitySpec&类型的参数
	BroadcastDelegate.BindLambda([this,AuraAbilitySystemComponent](const FGameplayAbilitySpec& AbilitySpec)
	{
		FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoByTag(AuraAbilitySystemComponent->GetAbilityTagFromSpec(AbilitySpec));
		Info.InputTag = AuraAbilitySystemComponent->GetInputTagFromSpec(AbilitySpec);	//设置两个Tag
		Info.AbilityTag = AuraAbilitySystemComponent->GetAbilityTagFromSpec(AbilitySpec);
		AbilityInfoDelegate.Broadcast(Info);	//广播信息
	});
	AuraAbilitySystemComponent->ForEachAbility(BroadcastDelegate);
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
