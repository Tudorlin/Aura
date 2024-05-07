// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LevelInfo.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FAuraLevelUpInfo
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly)
	int32 LevelRequirement = 0;	//升级所需经验

	UPROPERTY(EditDefaultsOnly)		//属性点
	int32 AttributePointAward = 1;

	UPROPERTY(EditDefaultsOnly)		//技能点
	int32 SpellPointAward = 1;
};
UCLASS()
class AURA_API ULevelInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TArray<FAuraLevelUpInfo>  LevelUpInformation;

	int32 FindLevelForXP(int32 XP) const;		//经验设定为一局获取经验总量可以升到多少级，通过该总量查找能升多少级
};
