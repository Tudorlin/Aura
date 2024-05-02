// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"
#include "AuraAbilitySystemGlobals.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraAbilitySystemGlobals : public UAbilitySystemGlobals		//此类包含了GAS的全局悉信息，通常在DefaultGame.ini中进行更改，设置完成之后获取的Context就会变成AllocGameplayEffectContext中的返回值，即使不是用自定义的Context进行声明
{
	GENERATED_BODY()

	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;		//用于返回自定义的EffectContext实例
};
