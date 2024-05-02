// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "AuraAbilityType.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Kismet/GameplayStatics.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"
#include "GameMode/AuraGamemodeBase.h"

UOverlayWidgetController* UAuraAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	if(APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject,0))	//获取玩家控制器
	{
		if(AAuraHUD* AuraHUD = Cast<AAuraHUD>(PC->GetHUD()))			//通过控制器获取HUD转换为AuraHUD
		{
			AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();	//通过控制器获取玩家状态
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();			//通过玩家状态获取玩家的能力组件
			UAttributeSet* AS = PS->GetAttributeSet();								//通过玩家状态获取玩家的属性集
			const FWidgetControllerParams WidgetControllerParams(PC,PS,ASC,AS);			
			return AuraHUD->GetOverlayWidgetController(WidgetControllerParams);
		}
	}
	return nullptr;
}

UAttributeMenuWidgetController* UAuraAbilitySystemLibrary::GetAttributeMenuWidgetController(
	const UObject* WorldContextObject)
{
	if(APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject,0))
	{
		if(AAuraHUD* AuraHUD = Cast<AAuraHUD>(PC->GetHUD()))
		{
			AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
			const FWidgetControllerParams WidgetControllerParams = FWidgetControllerParams(PC,PS,ASC,AS);
			return AuraHUD->GetAttributeMenuWidgetController(WidgetControllerParams);
		}
	}
	return nullptr;
}

void UAuraAbilitySystemLibrary::InitializeDefaultAttributes(const UObject* WorldContextObject,
	ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC)
{
	// AAuraGamemodeBase* AuraGameMode = Cast<AAuraGamemodeBase>(UGameplayStatics::GetGameMode(WorldContextObject));	//储存了敌人游戏模式中三种属性初始化所用的GE
	// if(AuraGameMode==nullptr) return;		获取操作被封装为函数库中的函数

	AActor* AvatarActor = ASC->GetAvatarActor();	//通过传入的能力组件获取到AvatarActor

	UCharacterClassInfo* CharacterClassInfo = GetCharacterInfo(WorldContextObject);		//用于获取次要和重要属性初始化的GE
	FCharacterClassDefaultInfo ClassDefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);	//用于获取初始化主要属性的GE，通过枚举在Map中查找对应的GE

	FGameplayEffectContextHandle PrimaryAttributesContextHandle = ASC->MakeEffectContext();
	PrimaryAttributesContextHandle.AddSourceObject(AvatarActor);			//一定要添加
	const FGameplayEffectSpecHandle PrimaryAttributesSpecHandle = ASC->MakeOutgoingSpec(ClassDefaultInfo.PrimaryAttributes,Level,PrimaryAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*PrimaryAttributesSpecHandle.Data.Get());		//应用效果

	FGameplayEffectContextHandle SecondaryAttributesContextHandle = ASC->MakeEffectContext();
	SecondaryAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle SecondaryAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->SecondaryAttributes,Level,SecondaryAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*SecondaryAttributesSpecHandle.Data.Get());

	FGameplayEffectContextHandle VitalAttributesContextHandle = ASC->MakeEffectContext();
	VitalAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle VitalAttributesHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->VitalAttributes,Level,VitalAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*VitalAttributesHandle.Data.Get());
}

void UAuraAbilitySystemLibrary::GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC)
{
	// AAuraGamemodeBase* AuraGameMode = Cast<AAuraGamemodeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	// if(AuraGameMode==nullptr) return;

	UCharacterClassInfo* CharacterClassInfo = GetCharacterInfo(WorldContextObject);	//获取游戏模式中的ClassInfo
	for(TSubclassOf<UGameplayAbility> Ability : CharacterClassInfo->CommonAbilities)	//遍历ClassInfo中储存的敌人的技能数组
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Ability,1.f);
		ASC->GiveAbility(AbilitySpec);				//将技能逐个赋予该敌人的能力组件。
	}
}

UCharacterClassInfo* UAuraAbilitySystemLibrary::GetCharacterInfo(const UObject* WorldContextObject)
{
	AAuraGamemodeBase* AuraGamemodeBase = Cast<AAuraGamemodeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if(AuraGamemodeBase==nullptr) return nullptr;
	return AuraGamemodeBase->CharacterClassInfo;
}

bool UAuraAbilitySystemLibrary::IsBlocked(const FGameplayEffectContextHandle& ContextHandle)
{
	if(const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(ContextHandle.Get()))		//注意参数声明为const,调用的Get返回的是const版本
	{
		return AuraEffectContext->IsBlocked();
	}
	return false;
}

bool UAuraAbilitySystemLibrary::IsCriticalHit(const FGameplayEffectContextHandle& ContextHandle)
{
	if(const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(ContextHandle.Get()))
	{
		return AuraEffectContext->IsCriticalHit();
	}
	return false;
}

void UAuraAbilitySystemLibrary::SetIsBlocked(FGameplayEffectContextHandle& ContextHandle, bool bInIsBlocked)
{
	if(FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(ContextHandle.Get()))	//参数没有声明const,可以直接转换
	{
		AuraEffectContext->SetIsBlocked(bInIsBlocked);
	}
}

void UAuraAbilitySystemLibrary::SetIsCriticalHit(FGameplayEffectContextHandle& ContextHandle,
	bool bInIsCriticalHit)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(ContextHandle.Get()))
	{
		AuraEffectContext->SetIsCriticalHit(bInIsCriticalHit);
	}
}
