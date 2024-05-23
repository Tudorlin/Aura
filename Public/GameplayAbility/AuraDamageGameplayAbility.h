// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraAbilityType.h"
#include "GameplayAbility/AuraGameplayAbility.h"
#include "Interfaction/CombatInterface.h"
#include "AuraDamageGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraDamageGameplayAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void CauseDamage(AActor* Target);
	
	UFUNCTION(BlueprintPure)
	FDamageEffectParams MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor = nullptr) const;		//设置DamageEffect结构体中的参数,便于在其他地方使用
	

protected:
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffectsClass;

	// UPROPERTY(EditDefaultsOnly,Category="Damage")
	// TMap<FGameplayTag,FScalableFloat>DamageTypes;
	
	UPROPERTY(EditDefaultsOnly,Category="Damage")
	FGameplayTag DamageType;
	
	UPROPERTY(EditDefaultsOnly,Category="Damage")
	FScalableFloat Damage;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DebuffChance = 20.f;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DebuffDamage = 5.f;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DebuffFrequency = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DebuffDuration = 5.f;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")		//敌人死亡时被附加的冲量
	float DeathImpulseMagnitude = 1000.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float KnockbackForceMagnitude = 1000.f;
    
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float KnockbackChance = 0.f;

	UFUNCTION(BlueprintPure)
	FTaggedMontage GetRandomMontage(const TArray<FTaggedMontage>& TargetArray) const;

	//float GetDamageByDamageType(float Level,const FGameplayTag& DamageType);
};
