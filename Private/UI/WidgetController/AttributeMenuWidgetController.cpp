// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"


void UAttributeMenuWidgetController::BroadcastInitialValue()
{
	const UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributesSet);   //AuraWidgetController中的属性集
	check(AttributeInfo);    //派生的蓝图中传入

	FAuraAttributeInfo Info=AttributeInfo->FindAttributeInfoForTag(FAuraGameplayTags::Get().Attributes_Primary_Strength);
	Info.AttributeValue = AS->GetStrength();
	AttributeInfoDelegate.Broadcast(Info);
}

void UAttributeMenuWidgetController::BindCallbacksDependencies()
{
	
}
