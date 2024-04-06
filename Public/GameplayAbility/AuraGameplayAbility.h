// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AuraGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()


public:
	UPROPERTY(EditDefaultsOnly,Category="Input")
	FGameplayTag StartupTag;     //每个技能自带Tag，用于传入能力组件中的函数激活能力用
};
