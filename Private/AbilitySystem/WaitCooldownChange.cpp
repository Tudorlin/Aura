// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/WaitCooldownChange.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"

UWaitCooldownChange* UWaitCooldownChange::WaitForCooldownChange(UAbilitySystemComponent* AbilitySystemComponent,
	const FGameplayTag& InCooldownTag)
{
	UWaitCooldownChange* WaitCooldownChange = NewObject<UWaitCooldownChange>();
	WaitCooldownChange->ASC = AbilitySystemComponent;
	WaitCooldownChange->CooldownTag = InCooldownTag;

	if (!IsValid(AbilitySystemComponent) || !InCooldownTag.IsValid())
	{
		WaitCooldownChange->EndTask();		//输入参数为空则直接清理掉此对象
		return nullptr;
	}

	// 注册CoolTag标签事件，ActiveGameplayEffect中标签被添加或移除时触发CooldownTagChanged函数
	AbilitySystemComponent->RegisterGameplayTagEvent(
		InCooldownTag,
		EGameplayTagEventType::NewOrRemoved).AddUObject(
			WaitCooldownChange,
			&UWaitCooldownChange::CooldownTagChanged);

	// ActiveGameplayEffect被添加时触发回调广播冷却时间
	AbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(WaitCooldownChange, &UWaitCooldownChange::OnActiveEffectAdded);

	return WaitCooldownChange;
}

void UWaitCooldownChange::EndTask()	//输入异常情况下的清理
{
	if(!IsValid(ASC)) return;

	ASC->RegisterGameplayTagEvent(CooldownTag,EGameplayTagEventType::NewOrRemoved).RemoveAll(this);	//注册Tag事件并移除监听

	SetReadyToDestroy();
	MarkAsGarbage();		//标记为垃圾，非UObject对象通过标记之后可被GC回收。
}

void UWaitCooldownChange::CooldownTagChanged(const FGameplayTag InCooldownTag, int32 NewCount)
{
	if(NewCount==0)
	{
		CooldownEnd.Broadcast(0.f);		//广播冷却时间结束
	}
}

void UWaitCooldownChange::OnActiveEffectAdded(UAbilitySystemComponent* TargetASC,
	const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer AssetTags;		//获取当前能力组件的AssetTag和GrantedTags
	SpecApplied.GetAllAssetTags(AssetTags);
	
	FGameplayTagContainer GrantedTags;
	SpecApplied.GetAllGrantedTags(GrantedTags);

	if(AssetTags.HasTagExact(CooldownTag)||GrantedTags.HasTagExact(CooldownTag))
	{
		//查询，给定的标签都在GE中则匹配成功;MakeQuery_MatchAllOwningTags为创建查询条件，
		FGameplayEffectQuery GameplayEffectQuery = FGameplayEffectQuery::MakeQuery_MatchAllOwningTags(CooldownTag.GetSingleTagContainer());
		TArray<float>TimesRemaining = ASC->GetActiveEffectsTimeRemaining(GameplayEffectQuery);//通过查询条件获取GE的剩余持续时间
		if(TimesRemaining.Num()>0)
		{
			float TimeRemaining = TimesRemaining[0];
			for(int32 i=0;i<TimesRemaining.Num();i++)
			{
				if(TimeRemaining<TimesRemaining[i])
				{
					TimeRemaining = TimesRemaining[i];
				}
			}
			CooldownStart.Broadcast(TimeRemaining);//广播开始冷却
		}
	}
}
