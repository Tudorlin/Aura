// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbility/AuraDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

void UAuraDamageGameplayAbility::CauseDamage(AActor* Target)
{
	FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectsClass,1.f);
	for(TTuple<FGameplayTag,FScalableFloat>Pair : DamageType)
	{
		const float ScaledDamage = Pair.Value.GetValueAtLevel(GetAbilityLevel());
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(EffectSpecHandle,Pair.Key,ScaledDamage);	//设置伤害值
	}
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*EffectSpecHandle.Data.Get(),
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target));	//作用与目标的ASC中
}

FTaggedMontage UAuraDamageGameplayAbility::GetRandomMontage(const TArray<FTaggedMontage>& TargetArray) const
{
	if(TargetArray.Num()>0)
	{
		const int32 Selection = FMath::RandRange(0,TargetArray.Num()-1);
		return TargetArray[Selection];
	}
	return FTaggedMontage();
}
