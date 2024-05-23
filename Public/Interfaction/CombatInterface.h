// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"

enum class ECharacterClass : uint8;
class UNiagaraSystem;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnASCRegistered, UAbilitySystemComponent*)

USTRUCT(BlueprintType)
struct FTaggedMontage	//将蒙太奇动画关联到Tag
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	UAnimMontage* Montage = nullptr;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FGameplayTag MontageTag;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FGameplayTag SocketTag;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	USoundBase* HitSound = nullptr;
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
	UFUNCTION(BlueprintNativeEvent)
	int32 GetPlayerLevel();


	UFUNCTION(BlueprintCallable,BlueprintNativeEvent) //注意CharacterBase继承了这个接口
	FVector GetCombatSocketLocation(const FGameplayTag& CombatSocketTag);

	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable)
	void UpdateFacingTarget(const FVector Target);    //用于降低MotionWarping与角色之间的耦合性，转化为接口类而不是角色

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	UAnimMontage* GetHitMontage();

	virtual void Die(const FVector& DeathImpulse) = 0;

	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	bool IsDead() const;

	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	AActor* GetAvatar();

	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	TArray<FTaggedMontage> GetAttackMontages();

	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	UNiagaraSystem* GetBloodEffect();
	
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	FTaggedMontage GetTaggedMontageByTag(const FGameplayTag& MontageTag);	//在Cue中使用，GA中传递给GC之后通过Tag获取对应的声音或粒子资产

	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	int32 GetMinionCount();		//获取已召唤的个数

	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	void IncrementMinionCount(int32 Amount);

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	ECharacterClass GetCharacterClass();

	virtual FOnASCRegistered GetOnASCRegisteredDelegate() = 0;
};
