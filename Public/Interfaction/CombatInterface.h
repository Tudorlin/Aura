// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"

USTRUCT(BlueprintType)
struct FTaggedMontage	//将蒙太奇动画关联到Tag
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	UAnimMontage* Montage = nullptr;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FGameplayTag MontageTag;
};

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


	UFUNCTION(BlueprintCallable,BlueprintNativeEvent) //注意CharacterBase继承了这个接口
	FVector GetCombatSocketLocation(const FGameplayTag& MontageTag);

	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable)
	void UpdateFacingTarget(const FVector Target);    //用于降低MotionWarping与角色之间的耦合性，转化为接口类而不是角色

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	UAnimMontage* GetHitMontage();

	virtual void Die() = 0;

	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	bool IsDead() const;

	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	AActor* GetAvatar();

	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	TArray<FTaggedMontage> GetAttackMontages();
};
