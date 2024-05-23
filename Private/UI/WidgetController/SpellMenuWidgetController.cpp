// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AuraPlayerState.h"

void USpellMenuWidgetController::BroadcastInitialValue()
{
	BroadcastAbilityInfo();
	SpellPointChanged.Broadcast(GetAuraPS()->GetSpellPoint());
}

void USpellMenuWidgetController::BindCallbacksDependencies()
{
	GetAuraASC()->AbilityStatusChanged.AddLambda([this](const FGameplayTag& AbilityTag,const FGameplayTag& StatusTag,int32 NewLevel)
	{
		if(SelectedAbility.AbilityTag.MatchesTag(AbilityTag))	//技能状态变化时检测技能是否选中以决定按钮是否启用
		{
			SelectedAbility.AbilityStatusTag = StatusTag;
			bool bEnableSpendPoints = false;
			bool bEnableEquip = false;			//当前的技能点数量在技能点数量改变时获取
			ShouldEnableButtons(SelectedAbility.AbilityStatusTag,
				CurrentSpellPoint,bEnableSpendPoints,bEnableEquip);
			FString Description = FString();
			FString NextLevelDescription = FString();
			GetAuraASC()->GetDescriptionsByAbilityTag(AbilityTag,
				Description,NextLevelDescription);	//通过能力组件获取描述
			SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPoints,bEnableEquip,
				Description,NextLevelDescription);	//广播到UI中
		}
		if(AbilityInfo)
		{
			FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoByTag(AbilityTag);
			Info.StatusTag = StatusTag;
			AbilityInfoDelegate.Broadcast(Info);		//这里的信息广播是由于技能状态的改变而触发
		}
	});
	GetAuraASC()->AbilityEquipped.AddUObject(this,&USpellMenuWidgetController::OnAbilityEquipped);	//能力组件中的装备委托注册函数
	
	GetAuraPS()->OnSpellPointChangedDelegate.AddLambda([this](int32 SpellPoints)
	{
		SpellPointChanged.Broadcast(SpellPoints);
		CurrentSpellPoint = SpellPoints;
		bool bEnableSpendPoints = false;
		bool bEnableEquip = false;
		ShouldEnableButtons(SelectedAbility.AbilityStatusTag,
			CurrentSpellPoint, bEnableSpendPoints, bEnableEquip);
		FString Description = FString();
		FString NextLevelDescription = FString();
		GetAuraASC()->GetDescriptionsByAbilityTag(SelectedAbility.AbilityTag,
		Description,NextLevelDescription);	//通过能力组件获取描述
		SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPoints,
			bEnableEquip,Description,NextLevelDescription);
	});
}

void USpellMenuWidgetController::SpellGlobeSelected(const FGameplayTag& AbilityTag)	//在技能树按钮的子控件中点击时调用
{
	if(bWaitingForEquipSelection)	//如果是待装备的状态
	{
		const FGameplayTag SelectedAbilityType = AbilityInfo->FindAbilityInfoByTag(AbilityTag).AbilityType;
		StopWaitingForEquipDelegate.Broadcast(SelectedAbilityType);	//停止动画的播放
		bWaitingForEquipSelection =	false;	//
	}
	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
	const int32 SpellPoints = GetAuraPS()->GetSpellPoint();	//获取技能点
	FGameplayTag AbilityStatus;

	const bool bTagValid = AbilityTag.IsValid();		//判定传入的Tag是否有效
	const bool bTagNone = AbilityTag.MatchesTag(GameplayTags.Abilities_None);//判定传入的Tag不是NOne
	const FGameplayAbilitySpec* AbilitySpec = GetAuraASC()->GetSpecFromAbilityTag(AbilityTag);	//通过GAS中的函数根据Tag获取对应的Spec
	const bool bSpecValid = AbilitySpec!=nullptr;
	if(!bTagValid||bTagNone||!bSpecValid)
	{
		AbilityStatus = GameplayTags.Abilities_Status_Locked;	//有一个为空则说明为锁定的状态
	}
	else
	{
		AbilityStatus = GetAuraASC()->GetStatusFromSpec(*AbilitySpec);//有效的话通过Spec获取技能的状态
	}
	SelectedAbility.AbilityTag = AbilityTag;
	SelectedAbility.AbilityStatusTag = AbilityStatus;
	bool bEnableSpendPoints = false;
	bool bEnableEquip = false;
	ShouldEnableButtons(AbilityStatus,SpellPoints,
		bEnableSpendPoints,bEnableEquip);	//确定按钮状态
	FString Description = FString();
	FString NextLevelDescription = FString();
	GetAuraASC()->GetDescriptionsByAbilityTag(AbilityTag,
		Description,NextLevelDescription);	//通过能力组件获取描述
	SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPoints,
		bEnableEquip,Description,NextLevelDescription);	//广播信息
}

void USpellMenuWidgetController::SpendPointButtonPressed()
{
	if(GetAuraASC())
	{
		GetAuraASC()->ServerSpendSpellPoint(SelectedAbility.AbilityTag);
	}
}

void USpellMenuWidgetController::GlobeDeselect()		//取消选中
{
	if(bWaitingForEquipSelection)	//如果此时是待装备的状态
	{
		const FGameplayTag SelectAbilityType = AbilityInfo->FindAbilityInfoByTag(SelectedAbility.AbilityTag).AbilityType;
		StopWaitingForEquipDelegate.Broadcast(SelectAbilityType);	//取消选中后停止动画的播放
		bWaitingForEquipSelection = false;
	}
	
	SelectedAbility.AbilityTag = FAuraGameplayTags::Get().Abilities_None;	//没有技能被选中
	SelectedAbility.AbilityStatusTag = FAuraGameplayTags::Get().Abilities_Status_Locked;	//选中的技能状态回到默认

	SpellGlobeSelectedDelegate.Broadcast(false, false, FString(), FString());//广播到菜单
}

void USpellMenuWidgetController::EquipButtonPressed()
{
	const FGameplayTag AbilityType = AbilityInfo->FindAbilityInfoByTag(SelectedAbility.AbilityTag).AbilityType;	//获取技能类型
	WaitForEquipDelegate.Broadcast(AbilityType);		//装备按钮按下后选中的技能进入待装备状态，广播判断是主动技能还是被动技能并播放相应的动画
	bWaitingForEquipSelection = true;		//

	const FGameplayTag SelectedStatus = GetAuraASC()->GetStatusFromAbilityTag(SelectedAbility.AbilityTag);	//获取选中技能的Status
	if(SelectedStatus.MatchesTag(FAuraGameplayTags::Get().Abilities_Status_Equipped))	//如果当前技能是已装备的状态
	{
		SelectedSlot = GetAuraASC()->GetInputTagFromAbilityTag(SelectedAbility.AbilityTag);		//将选中的插槽设定为选中技能的Tag;
	}
}

void USpellMenuWidgetController::SpellRowGlobePressed(const FGameplayTag& SlotTag, const FGameplayTag& AbilityType)	//点击主动和被动区域的图标按钮
{
	if(!bWaitingForEquipSelection) return;	//不处在技能装备状态则不触发

	const FGameplayTag& SelectAbilityType = AbilityInfo->FindAbilityInfoByTag(SelectedAbility.AbilityTag).AbilityType;
	if(!SelectAbilityType.MatchesTagExact(AbilityType)) return;

	GetAuraASC()->ServerEquipAbility(SelectedAbility.AbilityTag,SlotTag);	//处理技能装备，客户端的表现为广播信息
}

void USpellMenuWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& Status,
	const FGameplayTag& Slot, const FGameplayTag& PreviousSlot)
{
	bWaitingForEquipSelection = false;

	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();

	FAuraAbilityInfo LastSlotInfo;
	LastSlotInfo.StatusTag = GameplayTags.Abilities_Status_Unlocked;
	LastSlotInfo.InputTag = PreviousSlot;
	LastSlotInfo.AbilityTag = GameplayTags.Abilities_None;
	// 如果前一个插槽有效就广播空信息
	AbilityInfoDelegate.Broadcast(LastSlotInfo);		//清除旧的插槽

	FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoByTag(AbilityTag);
	Info.StatusTag = Status;
	Info.InputTag = Slot;
	AbilityInfoDelegate.Broadcast(Info);		//填充新的插槽

	StopWaitingForEquipDelegate.Broadcast(AbilityInfo->FindAbilityInfoByTag(AbilityTag).AbilityType);

	SpellGlobeReassignedDelegate.Broadcast(AbilityTag);	//广播装备的技能
	GlobeDeselect();		//取消选中
}

void USpellMenuWidgetController::ShouldEnableButtons(const FGameplayTag& AbilityStatus, int32 SpellPoints,
                                                     bool& bShouldEnableSpellPointsButton, bool& bShouldEnableEquipButton)
{
	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();

	bShouldEnableEquipButton = false;
	bShouldEnableSpellPointsButton = false;
	if(AbilityStatus.MatchesTag(GameplayTags.Abilities_Status_Equipped))		//技能是装备的情况下启用装备按钮
	{
		bShouldEnableEquipButton = true;
		if(SpellPoints>0)		//如果技能点大于0，启用加点按钮
		{
			bShouldEnableSpellPointsButton = true;
		}
	}
	else if(AbilityStatus.MatchesTag(GameplayTags.Abilities_Status_Eligible))	//技能处于可解锁状态
	{
		if(SpellPoints>0)		//技能点大于0的情况下启用加点按钮
		{
			bShouldEnableSpellPointsButton = true;
		}
	}
	else if(AbilityStatus.MatchesTag(GameplayTags.Abilities_Status_Unlocked))	//技能没锁定的情况
	{
		bShouldEnableEquipButton = true;
		if(SpellPoints>0)		//技能点大于0的情况下启用加点按钮
		{
			bShouldEnableSpellPointsButton = true;
		}
	}
}
