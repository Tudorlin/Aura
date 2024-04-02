// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_MaxHealth.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UMMC_MaxHealth : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	UMMC_MaxHealth();

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:
	FGameplayEffectAttributeCaptureDefinition VigorDef;//FGameplayEffectAttributeCaptureDefinition用于描述 Gameplay Effect 中的属性捕获选项。主要成员变量:
	//AttributeToCapture：要捕获的游戏属性。AttributeSource：游戏属性的来源（例如目标、施法者等）。bSnapshot：是否对属性进行快照。
};
