// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"


class UAuraGameplayAbility;
DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer& /*AssetTags*/)
DECLARE_MULTICAST_DELEGATE_OneParam(FAbilityGiven,UAuraAbilitySystemComponent*);	//技能初始化委托
DECLARE_DELEGATE_OneParam(FForEachAbility,const FGameplayAbilitySpec&);	//单播，这样定义是有一个参数
/**
 * 
 */
UCLASS()
class AURA_API UAuraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:
	void AbilityActorInfoSet();

	FEffectAssetTags EffectAssetTags;
	FAbilityGiven AbilitiesGivenDelegate;		//技能初始化完成时将角色的能力组件广播

	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities);
	void AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities);	//添加被动技能
	bool bStartupAbilitiesGiven = false;
	bool bStartupAbilities = false;		//判断技能初始化是否完成
	
	void AbilityInputTagHeld(const FGameplayTag& InputTag);    //按下按键时尝试激活能力
	void AbilityInputTagReleased(const FGameplayTag& InputTag);   //松开按键时将能力设置为未激活

	void ForEachAbility(const FForEachAbility& Delegate);

	static FGameplayTag GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
protected:
	UFUNCTION(Client,Reliable)
	void EffectApplied(UAbilitySystemComponent*AbilitySystemComponent,const FGameplayEffectSpec& EffectSpec,FActiveGameplayEffectHandle ActiveEffectHandle);

	virtual void OnRep_ActivateAbilities() override;
};
