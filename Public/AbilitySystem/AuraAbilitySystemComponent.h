// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"


class UAuraGameplayAbility;
DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer& /*AssetTags*/)
DECLARE_MULTICAST_DELEGATE(FAbilityGiven);	//技能初始化委托
DECLARE_DELEGATE_OneParam(FForEachAbility,const FGameplayAbilitySpec&);	//单播，这样定义是有一个参数
DECLARE_MULTICAST_DELEGATE_ThreeParams(FAbilityStatusChanged, const FGameplayTag& /*AbilityTag*/, const FGameplayTag& /*StatusTag*/,int32);
DECLARE_MULTICAST_DELEGATE_FourParams(FAbilityEquipped, const FGameplayTag& /*AbilityTag*/,
	const FGameplayTag& /*Status*/, const FGameplayTag& /*Slot*/, const FGameplayTag& /*PrevSlot*/);
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
	FAbilityStatusChanged AbilityStatusChanged;		//技能状态改变时的委托，通知控件控制器这个状态发生了改变
	FAbilityEquipped AbilityEquipped;		//技能完成装备

	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities);
	void AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities);	//添加被动技能
	bool bStartupAbilitiesGiven = false;//判断技能初始化是否完成
	
	void AbilityInputTagHeld(const FGameplayTag& InputTag);    //按下按键时尝试激活能力
	void AbilityInputTagReleased(const FGameplayTag& InputTag);   //松开按键时将能力设置为未激活

	void ForEachAbility(const FForEachAbility& Delegate);

	static FGameplayTag GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);		//输入Tag是自定义的变量
	static FGameplayTag GetStatusFromSpec(const FGameplayAbilitySpec& AbilitySpec);

	FGameplayTag GetStatusFromAbilityTag(const FGameplayTag& AbilityTag);
	FGameplayTag GetInputTagFromAbilityTag(const FGameplayTag& AbilityTag);

	FGameplayAbilitySpec* GetSpecFromAbilityTag(const FGameplayTag& AbilityTag);	//与上面的相反

	void UpgradeAttribute(const FGameplayTag& AttributeTag);
	UFUNCTION(Server,Reliable)
	void ServerUpgradeAttribute(const FGameplayTag& AttributeTag);

	void UpdateAbilityStatus(int32 Level);		//等级达到时更新技能的状态

	UFUNCTION(Server,Reliable)
	void ServerSpendSpellPoint(const FGameplayTag& AbilityTag);

	bool GetDescriptionsByAbilityTag(const FGameplayTag& AbilityTag,
		FString& OutDescription, FString& OutNextLevelDescription);		//通过Tag获取技能的描述

	UFUNCTION(Server, Reliable)
	void ServerEquipAbility(const FGameplayTag& AbilityTag, const FGameplayTag& Slot);	//服务器上装备

	void ClientEquipAbility(const FGameplayTag& AbilityTag, const FGameplayTag& Status,	//客户端上装备
		const FGameplayTag& Slot, const FGameplayTag& PreviousSlot);

	void ClearSlot(FGameplayAbilitySpec* Spec);		//如果选择装备的插槽已经有技能了，需要先将原插槽的技能清空
	void ClearAbilitiesOfSlot(const FGameplayTag& Slot);
	static bool AbilityHasSlot(FGameplayAbilitySpec* Spec, const FGameplayTag& Slot);
protected:
	UFUNCTION(Client,Reliable)
	void EffectApplied(UAbilitySystemComponent*AbilitySystemComponent,const FGameplayEffectSpec& EffectSpec,FActiveGameplayEffectHandle ActiveEffectHandle);

	UFUNCTION(Client,Reliable)
	void ClientUpdateAbilityStatus(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag,int32 AbilityLevel);

	virtual void OnRep_ActivateAbilities() override;
};
