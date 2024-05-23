// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "AbilityInfo.generated.h"

class UGameplayAbility;
/**
 * 
 */
USTRUCT(BlueprintType)
struct FAuraAbilityInfo	//技能的信息
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FGameplayTag AbilityTag = FGameplayTag();

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag InputTag = FGameplayTag();

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag StatusTag = FGameplayTag();		//技能状态Tag

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TObjectPtr<const UTexture2D> Icon = nullptr;		//技能图标

	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)		//技能类型,主动还是被动
	FGameplayTag AbilityType = FGameplayTag();

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TObjectPtr<const UMaterialInstance> BackGroundMaterial = nullptr;		//图标背景

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FGameplayTag CoolDownTag = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)		//技能需要的等级
	int32 LevelRequirement = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)		//技能本身
	TSubclassOf<UGameplayAbility> Ability;
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
