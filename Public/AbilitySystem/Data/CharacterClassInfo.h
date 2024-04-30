// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "Engine/DataAsset.h"
#include "CharacterClassInfo.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class ECharacterClass : uint8
{
	Elementalist,		//法师
	Warrior,				//战士
	Ranger			//游侠
};

USTRUCT(BlueprintType)
struct FCharacterClassDefaultInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly,Category="Class Default")
	TSubclassOf<UGameplayEffect> PrimaryAttributes;
};


UCLASS()
class AURA_API UCharacterClassInfo : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly,Category="Class Default")
	TMap<ECharacterClass,FCharacterClassDefaultInfo>CharacterClassInformation;  //不同的角色类型对应不同的初始主属性

	UPROPERTY(EditDefaultsOnly,Category="Class Default")
	TSubclassOf<UGameplayEffect> SecondaryAttributes;

	UPROPERTY(EditDefaultsOnly,Category="Class Default")
	TSubclassOf<UGameplayEffect> VitalAttributes;

	FCharacterClassDefaultInfo GetClassDefaultInfo(ECharacterClass CharacterClass);

	UPROPERTY(EditAnywhere,Category="Class Default")
	TArray<TSubclassOf<UGameplayAbility>> CommonAbilities;

	UPROPERTY(EditDefaultsOnly,Category="Class Default | Damage")
	TObjectPtr<UCurveTable> DamageCalculationCoefficient;
};
