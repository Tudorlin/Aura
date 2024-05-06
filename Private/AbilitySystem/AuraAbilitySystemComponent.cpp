// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AuraGameplayTags.h"
#include "GameplayAbility/AuraGameplayAbility.h"


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
	bStartupAbilities  = true;
	AbilitiesGivenDelegate.Broadcast(this);	//广播能力组件
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

void UAuraAbilitySystemComponent::ForEachAbility(const FForEachAbility& Delegate)
{
	FScopedAbilityListLock ActiveScopeLock(*this);		//能力锁，在遍历时锁定组件，防止在遍历过程中移除能力，生命周期结束时自动解锁
	for(const FGameplayAbilitySpec AbilitySpec : GetActivatableAbilities())
	{
		if(!Delegate.ExecuteIfBound(AbilitySpec))		//如果该委托有绑定回调就触发否则就打印Log
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to execute delegate in %hs"), __FUNCTION__);
		}
	}
}

FGameplayTag UAuraAbilitySystemComponent::GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if(AbilitySpec.Ability)
	{
		for(FGameplayTag Tag : AbilitySpec.Ability.Get()->AbilityTags)
		{
			if(Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities"))))
			{
				return Tag;
			}
		}
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if(AbilitySpec.Ability)
	{
		for(FGameplayTag Tag : AbilitySpec.DynamicAbilityTags)
		{
			if(Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Input"))))	//注意这里名字跟教程中的不一样，需要按自己的来
			{
				return Tag;
			}
		}
	}
	return FGameplayTag();
}

void UAuraAbilitySystemComponent::OnRep_ActivateAbilities()	//ActivateAbilities即被激活的能力发生改变时触发,不重写的话广播只会在服务器上执行，客户端无法收到消息
{
	Super::OnRep_ActivateAbilities();
	if(!bStartupAbilities)
	{
		bStartupAbilities = true;
		AbilitiesGivenDelegate.Broadcast(this);
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
