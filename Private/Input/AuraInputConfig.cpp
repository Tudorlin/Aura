// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/AuraInputConfig.h"

const UInputAction* UAuraInputConfig::FindAbilityInputActionTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for(const auto& Action : AbilityInputActions)
	{
		if(Action.InputAction&&Action.InputTag==InputTag)
		{
			return Action.InputAction;
		}
		if(bLogNotFound)
		{
			UE_LOG(LogTemp,Error,TEXT("未在[%s]中找到tag[%s]"),*GetNameSafe(this),*InputTag.ToString());
		}
	}
	return nullptr;
}
