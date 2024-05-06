// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "AbilityInfo.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FAuraAbilityInfo	//能力的信息，暂用于UI通信
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FGameplayTag AbilityTag = FGameplayTag();

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag InputTag = FGameplayTag();

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TObjectPtr<const UTexture2D> Icon = nullptr;		//技能图标

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TObjectPtr<const UMaterialInstance> BackGroundMaterial = nullptr;		//图标背景

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FGameplayTag CoolDownTag = FGameplayTag();
};

UCLASS()
class AURA_API UAbilityInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TArray<FAuraAbilityInfo> AbilityInformation;

	FAuraAbilityInfo FindAbilityInfoByTag(const FGameplayTag& AbilityTag,bool bLogNotFound = false) const;	//根据Tag查找对应信息
};
