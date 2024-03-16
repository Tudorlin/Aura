// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/AuraWidgetController.h"

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
