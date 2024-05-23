// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbility/AuraDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilityType.h"

void UAuraDamageGameplayAbility::CauseDamage(AActor* Target)
{
	FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectsClass,1.f);
	const float ScaledDamage = Damage.GetValueAtLevel(GetAbilityLevel());
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(EffectSpecHandle,DamageType,ScaledDamage);	//设置伤害值
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*EffectSpecHandle.Data.Get(),
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target));	//作用与目标的ASC中
}

FDamageEffectParams UAuraDamageGameplayAbility::MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor) const
{
	FDamageEffectParams Params;
	Params.WorldContextObject = GetAvatarActorFromActorInfo();
	Params.DamageGameplayEffectClass = DamageEffectsClass;
	Params.SourceAbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();	//源能力组件
	Params.TargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);	//目标能力组件
	Params.BaseDamage = Damage.GetValueAtLevel(GetAbilityLevel());
	Params.AbilityLevel = GetAbilityLevel();
	Params.DamageType = DamageType;
	Params.DebuffChance = DebuffChance;
	Params.DebuffDamage = DebuffDamage;
	Params.DebuffDuration = DebuffDuration;
	Params.DebuffFrequency = DebuffFrequency;
	Params.DeathImpulseMagnitude = DeathImpulseMagnitude;
	Params.KnockbackForceMagnitude = KnockbackForceMagnitude;
	Params.KnockbackChance = KnockbackChance;
	if (IsValid(TargetActor))
	{
		FRotator Rotation = (TargetActor->GetActorLocation() - GetAvatarActorFromActorInfo()->GetActorLocation()).Rotation();
		Rotation.Pitch = 45.f;
		const FVector ToTarget = Rotation.Vector();
		Params.DeathImpulse = ToTarget * DeathImpulseMagnitude;
		Params.KnockbackForce = ToTarget * KnockbackForceMagnitude;
	}
	return Params;
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

// float UAuraDamageGameplayAbility::GetDamageByDamageType(float Level, const FGameplayTag& DamageType)
// {
// 	checkf(DamageTypes.Contains(DamageType),TEXT("GameplayAbilit [%s] "
// 	"does not contain DamageType [%s]"), *GetNameSafe(this), *DamageType.ToString());
// 	return DamageTypes[DamageType].GetValueAtLevel(Level);	//通过Tag获取对应的伤害值
// }			//将伤害类型从Map储存改成了一个单独的Tag和一个ScaleFloat
