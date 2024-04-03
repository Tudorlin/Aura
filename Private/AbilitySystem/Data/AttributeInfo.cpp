// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Data/AttributeInfo.h"

FAuraAttributeInfo UAttributeInfo::FindAttributeInfoForTag(const FGameplayTag& AttributeTag, bool bLogNotFound) const
{
	for(auto Info : AttributeInformation)
	{
		if(Info.AttributeTag.MatchesTag(AttributeTag))
			return Info;
	}
	if(bLogNotFound)
	{
		UE_LOG(LogTemp,Error,TEXT("未在[%s]中找到tag[%s]"),*GetNameSafe(this),*AttributeTag.ToString());
	}
	return FAuraAttributeInfo();
}
