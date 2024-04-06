// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AuraGameplayTags.h"
#include "..\..\Public\GameplayAbility"

void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this,&UAuraAbilitySystemComponent::EffectApplied);//有效果被应用到能力组件时触发回调
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
}

void UAuraAbilitySystemComponent::AddCharacterAbilities(
	const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
	for(const auto AbilityClass : StartupAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass,1.f);
		//GiveAbility(AbilitySpec);
		if(const UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec.Ability))
		{
			AbilitySpec.DynamicAbilityTags.AddTag(AuraAbility->StartupTag);   //添加tag到容器中用于检索
			GiveAbility(AbilitySpec);
		}
	}
		
}

void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)    //这两个函数会在控制器中通过按键绑定的函数进行调用
{
	if(!InputTag.IsValid()) return;

	for(FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())   //获取可以被激活的能力
	{
		if(AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))       //这里触发的时候遇到了个小问题，bebug发现输入的Tag与容器中Tag不相等，原因是之前只在代码改了Tag名，输入操作蓝图中的数据资产用的还是旧的Tag，由于改名旧的Tag变成无效Tag了
		{
			if(!AbilitySpec.IsActive())     //如果此能力未被激活则用Spec的句柄尝试激活该能力
				TryActivateAbility(AbilitySpec.Handle);
		}
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if(!InputTag.IsValid()) return;
	for(FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if(AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputReleased(AbilitySpec);   //处理输入释放事件的逻辑，例如将能力设置为未激活
		}
	}
}

void UAuraAbilitySystemComponent::EffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent,
                                                const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);   //将tag储存在传入的TagContainer中
	//将tag广播到Widget控制器
	EffectAssetTags.Broadcast(TagContainer);
}
