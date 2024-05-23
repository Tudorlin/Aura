// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystem/AuraAttributeSet.h"
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

	virtual FString GetDescription(int32 Level);
	virtual FString GetNextLevelDescription(int32 Level);
	static FString GetLockedDescription(int32 Level);

protected:
	float GetManaCost(float InLevel = 1.f) const;
	float GetCooldown(float InLevel = 1.f) const;
	// UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Damage")
	// FScalableFloat Damage;		将在DamageGameplayAbility中处理伤害
};

