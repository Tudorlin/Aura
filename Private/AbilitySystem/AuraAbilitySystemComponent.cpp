// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "GameplayAbility/AuraGameplayAbility.h"
#include "Interfaction/PlayerInterface.h"


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
			AbilitySpec.DynamicAbilityTags.AddTag(AuraAbility->StartupTag);   //添加tag到容器中用于在下面获取对应类型的Tag的检索
			AbilitySpec.DynamicAbilityTags.AddTag(FAuraGameplayTags::Get().Abilities_Status_Equipped);//技能附加给玩家之后处于装备状态。
			GiveAbility(AbilitySpec);
		}
	}
	bStartupAbilitiesGiven  = true;
	AbilitiesGivenDelegate.Broadcast();	//广播能力组件
}

void UAuraAbilitySystemComponent::AddCharacterPassiveAbilities(
	const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupPassiveAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass,1.f);
		GiveAbilityAndActivateOnce(AbilitySpec);
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
		for(FGameplayTag Tag : AbilitySpec.DynamicAbilityTags)		//储存了AbilitySpec上设置的所有Tag
		{
			if(Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Input"))))	//注意这里名字跟教程中的不一样，需要按自己的来
			{
				return Tag;
			}
		}
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetStatusFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if(AbilitySpec.Ability)
	{
		for(FGameplayTag Tag : AbilitySpec.DynamicAbilityTags)
		{
			if(Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities.Status"))))
			{
				return Tag;
			}
		}
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetStatusFromAbilityTag(const FGameplayTag& AbilityTag)//首先从Tag获取Spec然后再获取状态Tag
{
	if(const FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		return GetStatusFromSpec(*AbilitySpec);		//通过Spec获取状态
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetInputTagFromAbilityTag(const FGameplayTag& AbilityTag)
{
	if(const FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		return GetInputTagFromSpec(*AbilitySpec);	//通过Spec获取输入Tag
	}
	return FGameplayTag();
}

void UAuraAbilitySystemComponent::ServerEquipAbility_Implementation(const FGameplayTag& AbilityTag,
	const FGameplayTag& Slot)
{
	if(FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
		const FGameplayTag& PrevSlot = GetInputTagFromSpec(*AbilitySpec);
		const FGameplayTag& Status = GetStatusFromSpec(*AbilitySpec);

		const bool bStatusValid = Status == GameplayTags.Abilities_Status_Equipped
			|| Status == GameplayTags.Abilities_Status_Unlocked;
		if(bStatusValid)		//状态是装备或者已解锁时才执行
		{
			ClearAbilitiesOfSlot(Slot);		//将此插槽中的技能移除
			ClearSlot(AbilitySpec);			//如果待装备的技能已经有插槽了也也将其移除
			AbilitySpec->DynamicAbilityTags.AddTag(Slot);	//由于上面移除了插槽在这里添加进去
			if(Status.MatchesTagExact(GameplayTags.Abilities_Status_Unlocked))	//如果技能是解锁状态
			{
				AbilitySpec->DynamicAbilityTags.RemoveTag(GameplayTags.Abilities_Status_Unlocked);	//移除已解锁并添加装备Tag
				AbilitySpec->DynamicAbilityTags.AddTag(GameplayTags.Abilities_Status_Equipped);
			}
			MarkAbilitySpecDirty(*AbilitySpec);		//同步更新
		}
		ClientEquipAbility(AbilityTag,GameplayTags.Abilities_Status_Equipped,Slot,PrevSlot);	//通过多播广播到控制器中
	}
}

void UAuraAbilitySystemComponent::ClientEquipAbility(const FGameplayTag& AbilityTag, const FGameplayTag& Status,
	const FGameplayTag& Slot, const FGameplayTag& PreviousSlot)
{
	AbilityEquipped.Broadcast(AbilityTag,Status,Slot,PreviousSlot);	//广播到技能菜单控制器类中
}

void UAuraAbilitySystemComponent::ClearSlot(FGameplayAbilitySpec* Spec)
{
	const FGameplayTag Slot = GetInputTagFromSpec(*Spec);	//将输入Tag作为Slot的标识
	Spec->DynamicAbilityTags.RemoveTag(Slot);	//移除掉此输入Tag
	MarkAbilitySpecDirty(*Spec);		//进行同步
}

void UAuraAbilitySystemComponent::ClearAbilitiesOfSlot(const FGameplayTag& Slot)
{
	FScopedAbilityListLock ActiveScopeLock(*this);	//上锁
	for(FGameplayAbilitySpec& Spec : GetActivatableAbilities())	//遍历激活的技能。
	{
		if(AbilityHasSlot(&Spec,Slot))		//查找到有此Slot就将其清理
		{
			ClearSlot(&Spec);
		}
	}
}

bool UAuraAbilitySystemComponent::AbilityHasSlot(FGameplayAbilitySpec* Spec, const FGameplayTag& Slot)
{
	for(FGameplayTag Tag : Spec->DynamicAbilityTags)		//在技能spec中查找是否含有Slot标识
	{
		if(Tag.MatchesTag(Slot))
		{
			return true;
		}
	}
	return false;
}

FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetSpecFromAbilityTag(const FGameplayTag& AbilityTag)
{
	FScopedAbilityListLock ActiveScopeLoc(*this);	//上锁
	for(FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())	//获取可以被激活的技能
	{
		for(FGameplayTag Tag: AbilitySpec.Ability.Get()->AbilityTags)		//通过技能Tag寻找指定的Spec
		{
			if(Tag.MatchesTag(AbilityTag))
				return &AbilitySpec;
		}
	}
	return nullptr;
}

void UAuraAbilitySystemComponent::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	if (GetAvatarActor()->Implements<UPlayerInterface>())
	{
		if (IPlayerInterface::Execute_GetAttributePoints(GetAvatarActor()) > 0)
		{
			ServerUpgradeAttribute(AttributeTag);
		}
	}
}

void UAuraAbilitySystemComponent::UpdateAbilityStatus(int32 Level)
{
	UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());//从GameMode中获取技能信息
	for(const FAuraAbilityInfo& Info: AbilityInfo->AbilityInformation)
	{
		if(!Info.AbilityTag.IsValid()) continue;
		if(Level<Info.LevelRequirement) continue;	//传入等级小于需求等级时跳过该次循环
		if(GetSpecFromAbilityTag(Info.AbilityTag)==nullptr)
		{
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Info.Ability,1);//哇如果获取到的Spec为空就根据信息中的Ability创建一个
			AbilitySpec.DynamicAbilityTags.AddTag(FAuraGameplayTags::Get().Abilities_Status_Eligible);	//标记为可解锁
			GiveAbility(AbilitySpec);
			MarkAbilitySpecDirty(AbilitySpec);		//该能力从锁定状态转变为可解锁，标记此Spec已经被改变过,主要用于网络同步，标记为Dirty之后就会在下一次更新的时候同步到客户端
			ClientUpdateAbilityStatus(Info.AbilityTag,FAuraGameplayTags::Get().Abilities_Status_Eligible,1);	//通知客户端进行广播
		}
	}
}

bool UAuraAbilitySystemComponent::GetDescriptionsByAbilityTag(const FGameplayTag& AbilityTag, FString& OutDescription,
	FString& OutNextLevelDescription)
{
	if(FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))	//通过Tag获取到能力
	{
		if(UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec->Ability))	//转化为Aura能力调用自定义的获取描述函数
		{
			OutDescription = AuraAbility->GetDescription(AbilitySpec->Level);		//当前等级的描述
			OutNextLevelDescription = AuraAbility->GetNextLevelDescription(AbilitySpec->Level+1);		//下一级的描述
			return true;
		}
	}
	const UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());	
	if (!AbilityTag.IsValid() || AbilityTag.MatchesTagExact(FAuraGameplayTags::Get().Abilities_None))	//取消选中技能
	{
		OutDescription = FString();
	}
	else				//未找到能力的话说明还没解锁，显示解锁需要的等级
	{
		OutDescription = UAuraGameplayAbility::GetLockedDescription(AbilityInfo->FindAbilityInfoByTag(AbilityTag).LevelRequirement);
	}
	OutNextLevelDescription = FString();
	return false;
}

void UAuraAbilitySystemComponent::ServerSpendSpellPoint_Implementation(const FGameplayTag& AbilityTag)	//在按下加点按钮的时候触发
{
	if(FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		if (GetAvatarActor()->Implements<UPlayerInterface>())
		{
			IPlayerInterface::Execute_AddToSpellPoints(GetAvatarActor(), -1);	//服务器上消耗技能点
		}

		const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
		FGameplayTag Status = GetStatusFromSpec(*AbilitySpec);
		if (Status.MatchesTagExact(GameplayTags.Abilities_Status_Eligible))	//按下加点按钮时如果技能是可解锁
		{
			AbilitySpec->DynamicAbilityTags.RemoveTag(GameplayTags.Abilities_Status_Eligible);	//移除可解锁的Tag
			AbilitySpec->DynamicAbilityTags.AddTag(GameplayTags.Abilities_Status_Unlocked);		//将技能状态改为解锁
			Status = GameplayTags.Abilities_Status_Unlocked;
		}
		else if (Status.MatchesTagExact(GameplayTags.Abilities_Status_Equipped) || Status.MatchesTagExact(GameplayTags.Abilities_Status_Unlocked))
		{
			AbilitySpec->Level += 1;		//如果选中的技能是已装备或者已解锁的，就将技能的等级加一
		}
		ClientUpdateAbilityStatus(AbilityTag, Status, AbilitySpec->Level);	//更新客户端上的技能状态,广播到UI中
		MarkAbilitySpecDirty(*AbilitySpec);	//用于同步到客户端
	}
}

void UAuraAbilitySystemComponent::ServerUpgradeAttribute_Implementation(const FGameplayTag& AttributeTag)
{
	FGameplayEventData Payload;			//发送属性点相关事件
	Payload.EventTag = AttributeTag;
	Payload.EventMagnitude = 1.f;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActor(), AttributeTag, Payload);	//通过监听GA应用到角色身上

	if (GetAvatarActor()->Implements<UPlayerInterface>())
	{
		IPlayerInterface::Execute_AddToAttributePoints(GetAvatarActor(), -1);
	}
}

void UAuraAbilitySystemComponent::ClientUpdateAbilityStatus_Implementation(const FGameplayTag& AbilityTag,
	const FGameplayTag& StatusTag,int32 AbilityLevel)
{
	AbilityStatusChanged.Broadcast(AbilityTag,StatusTag,AbilityLevel);
}

void UAuraAbilitySystemComponent::OnRep_ActivateAbilities()	//ActivateAbilities即被激活的能力发生改变时触发,不重写的话广播只会在服务器上执行，客户端无法收到消息
{
	Super::OnRep_ActivateAbilities();
	if(!bStartupAbilitiesGiven)
	{
		bStartupAbilitiesGiven = true;
		AbilitiesGivenDelegate.Broadcast();
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
