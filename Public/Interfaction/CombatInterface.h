// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI,BlueprintType)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};
class UAnimMontage;
/**
 * 
 */
class AURA_API ICombatInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual int32 GetPlayerLevel();
	virtual FVector GetCombatSocketLocation();  //注意CharacterBase继承了这个接口

	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable)
	void UpdateFacingTarget(const FVector Target);    //用于降低MotionWarping与角色之间的耦合性，转化为接口类而不是角色

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	UAnimMontage* GetHitMontage();

	virtual void Die() = 0;
};
