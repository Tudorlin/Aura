// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/MMC_MaxHealth.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "Interfaction/CombatInterface.h"

UMMC_MaxHealth::UMMC_MaxHealth()
{
	VigorDef.AttributeToCapture = UAuraAttributeSet::GetVigorAttribute();
	VigorDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	VigorDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(VigorDef);  //位于UGameplayModMagnitudeCalculation中的数组，储存了被捕获的属性
}

float UMMC_MaxHealth::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedSourceTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;  //用于在 Gameplay Effect 的聚合器（Aggregator）评估过程中传递数据
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float Vigor = 0.f;
	GetCapturedAttributeMagnitude(VigorDef,Spec,EvaluateParameters,Vigor);  //用于获取Vigor的值
	Vigor = FMath::Max<float>(Vigor,0.f);  //保证Vigor不小于0

	//ICombatInterface* CombatInterface = Cast<ICombatInterface>(Spec.GetContext().GetSourceObject());声明NativeEvent后需要指定调用版本
	//const int32 PlayerLevel = CombatInterface->GetPlayerLevel();
	int32 PlayerLevel = 1;	//获取等级
	if(Spec.GetContext().GetSourceObject()->Implements<UCombatInterface>())
	{
		PlayerLevel = ICombatInterface::Execute_GetPlayerLevel(Spec.GetContext().GetSourceObject());
	}

	return 80.f+2.5f*Vigor+10.f*PlayerLevel;  //返回计算结果
}
