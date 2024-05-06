// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "WaitCooldownChange.generated.h"

/**
 * 
 */

struct FActiveGameplayEffectHandle;
struct FGameplayEffectSpec;
class UAbilitySystemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCooldownChangeSignature, float, TimeRemaining);//返回冷却时间
UCLASS(BlueprintType,meta=(ExposedAsyncProxy="AsyncTask"))
class AURA_API UWaitCooldownChange : public UBlueprintAsyncActionBase		//异步加载请求
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)		//冷却开始时
	FCooldownChangeSignature CooldownStart;

	UPROPERTY(BlueprintAssignable)
	FCooldownChangeSignature CooldownEnd;		//冷却结束时

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))	//BlueprintInternalUseOnly表示该函数为内部函数
	static UWaitCooldownChange* WaitForCooldownChange(UAbilitySystemComponent* AbilitySystemComponent,
		const FGameplayTag& InCooldownTag);

	UFUNCTION(BlueprintCallable)
	void EndTask();

protected:
	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> ASC;

	FGameplayTag CooldownTag;	//冷却Tag标记

	void CooldownTagChanged(const FGameplayTag InCooldownTag, int32 NewCount);	//冷却结束，返回冷却时间0
	void OnActiveEffectAdded(UAbilitySystemComponent* TargetASC, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveEffectHandle);//技能激活时触发冷却
};
