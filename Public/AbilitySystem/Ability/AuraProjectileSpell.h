// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbility/AuraDamageGameplayAbility.h"
#include "AuraProjectileSpell.generated.h"

class AAuraProjectile;
class FGameplayEffect;
/**
 * 
 */
UCLASS()
class AURA_API UAuraProjectileSpell : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()


protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION(BlueprintCallable,Category="Projectile")    //可在蓝图中调用,用于在GE中接受到通知时触发，便于在蒙太奇播放到指定位置时生成火球
	void SpawnProjectile(const FVector& ProjectileTargetLocation,const FGameplayTag& SocketTag);

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)   
	TSubclassOf<AAuraProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly)
	int32 NumProjectiles = 5;
	
	// UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	// TSubclassOf<UGameplayEffect> DamageEffectClass;	//需要对伤害类型做出区别,无需传入单一的GE了
};
