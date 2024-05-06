// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Data/AbilityInfo.h"

FAuraAbilityInfo UAbilityInfo::FindAbilityInfoByTag(const FGameplayTag& AbilityTag, bool bLogNotFound) const
{
	 for(const FAuraAbilityInfo& Info : AbilityInformation)
	 {
		 if(Info.AbilityTag ==AbilityTag)
		 {
			 return Info;
		 }
	 }
	if(bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't find info for AbilityTag [%s] on AbilityInfo [%s]"), *AbilityTag.ToString(), *GetNameSafe(this));
	}
	return FAuraAbilityInfo();
}
