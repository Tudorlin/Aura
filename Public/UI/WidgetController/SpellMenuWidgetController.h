// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraGameplayTags.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "SpellMenuWidgetController.generated.h"

/**
 * 技能页面同样需要技能的信息，在做主UI的时候已经完成过一次将技能信息广播到OverlayWidgetController中了，
 * 所以要先将广播的逻辑移到最开始的基类中，便于在这里复用
 */

struct FAuraGameplayTags;

struct FSelectedAbility
{
	FGameplayTag AbilityTag = FGameplayTag();
	FGameplayTag AbilityStatusTag = FGameplayTag();
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FSpellGlobeSelectedSignature,bool,bSpendPointsButtonEnabled
	,bool,bEquipButtonEnabled,FString,OutDescription,FString,OutNextLevelDescription);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitForEquipSelectionSignature, const FGameplayTag&, AbilityType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSpellGlobeReassignedSignature, const FGameplayTag&, AbilityTag);

UCLASS(BlueprintType,Blueprintable)
class AURA_API USpellMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadcastInitialValue() override;
	virtual void BindCallbacksDependencies() override;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerStateChangedSignature SpellPointChanged;	//技能点变化委托

	UPROPERTY(BlueprintAssignable)
	FWaitForEquipSelectionSignature WaitForEquipDelegate;	//待装备委托

	UPROPERTY(BlueprintAssignable)
	FWaitForEquipSelectionSignature StopWaitingForEquipDelegate;	//停止装备，即装备完成或取消了技能的选中

	UPROPERTY(BlueprintAssignable)
	FSpellGlobeSelectedSignature SpellGlobeSelectedDelegate;	//技能描述以及消耗技能点和装备按钮是否启用委托

	UPROPERTY(BlueprintAssignable)
	FSpellGlobeReassignedSignature SpellGlobeReassignedDelegate;


	UFUNCTION(BlueprintCallable)
	void SpellGlobeSelected(const FGameplayTag& AbilityTag);
	
	UFUNCTION(BlueprintCallable)
	void SpendPointButtonPressed();		//加点按钮按下时

	UFUNCTION(BlueprintCallable)		//取消选中按钮
	void GlobeDeselect();

	UFUNCTION(BlueprintCallable)		//按下装备按钮
	void EquipButtonPressed();

	UFUNCTION(BlueprintCallable)
	void SpellRowGlobePressed(const FGameplayTag& SlotTag, const FGameplayTag& AbilityType);		//技能装备区域按下时

	void OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& Status,
		const FGameplayTag& Slot, const FGameplayTag& PreviousSlot);

private:
	static void ShouldEnableButtons(const FGameplayTag& AbilityStatus, int32 SpellPoints,
		bool& bShouldEnableSpellPointsButton, bool& bShouldEnableEquipButton);	//判断什么时候启用按钮

	FSelectedAbility SelectedAbility = {FAuraGameplayTags::Get().Abilities_None,
		FAuraGameplayTags::Get().Abilities_Status_Locked};
	int32 CurrentSpellPoint = 0;	//跟上面的结构体一起处理未解锁时选中按钮升级之后的按钮更新。

	bool bWaitingForEquipSelection = false;

	FGameplayTag SelectedSlot;		//选中的技能槽
};
