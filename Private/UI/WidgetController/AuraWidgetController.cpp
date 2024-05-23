// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/AuraWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"

void UAuraWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WCParams)
{
	PlayerController = WCParams.PlayerController;
	PlayerState = WCParams.PlayerState;
	AbilitySystemComponent = WCParams.AbilitySystemComponent;
	AttributesSet = WCParams.AttributeSet;
}

void UAuraWidgetController::BroadcastInitialValue()
{
}

void UAuraWidgetController::BindCallbacksDependencies()
{
}

void UAuraWidgetController::BroadcastAbilityInfo()
{
	if(!GetAuraASC()->bStartupAbilitiesGiven) return;
	//每个技能初始化时创建一个委托并绑定表达式，在能力组件类中调用ForEachAbility触发回调
	FForEachAbility BroadcastDelegate;		//传入一个AbilitySpec&类型的参数
	BroadcastDelegate.BindLambda([this](const FGameplayAbilitySpec& AbilitySpec)
	{
		FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoByTag(AuraAbilitySystemComponent->GetAbilityTagFromSpec(AbilitySpec));
		Info.InputTag = AuraAbilitySystemComponent->GetInputTagFromSpec(AbilitySpec);	//设置Tag
		Info.StatusTag = AuraAbilitySystemComponent->GetStatusFromSpec(AbilitySpec);
		AbilityInfoDelegate.Broadcast(Info);	//这里的广播用于初始化时设置图标
	});
	GetAuraASC()->ForEachAbility(BroadcastDelegate);
}


//获取基本的控制器、状态、ASC和AS
AAuraPlayerController* UAuraWidgetController::GetAuraPC()
{
	if (AuraPlayerController == nullptr)
		AuraPlayerController = Cast<AAuraPlayerController>(PlayerController);
	return AuraPlayerController;
}

AAuraPlayerState* UAuraWidgetController::GetAuraPS()
{
	if (AuraPlayerState == nullptr)
		AuraPlayerState = Cast<AAuraPlayerState>(PlayerState);
	return AuraPlayerState;
}

UAuraAbilitySystemComponent* UAuraWidgetController::GetAuraASC()
{
	if(AuraAbilitySystemComponent==nullptr)
		return AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent);
	return AuraAbilitySystemComponent;
}

UAuraAttributeSet* UAuraWidgetController::GetAuraAS()
{
	if(AuraAttributeSet==nullptr)
		return AuraAttributeSet = Cast<UAuraAttributeSet>(AttributesSet);
	return AuraAttributeSet;
}
