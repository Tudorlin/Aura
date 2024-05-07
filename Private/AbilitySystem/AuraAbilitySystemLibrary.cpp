// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "AuraAbilityType.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Kismet/GameplayStatics.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"
#include "GameMode/AuraGamemodeBase.h"
#include "Interfaction/CombatInterface.h"

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

void UAuraAbilitySystemLibrary::GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC,ECharacterClass CharacterClass)
{
	// AAuraGamemodeBase* AuraGameMode = Cast<AAuraGamemodeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	// if(AuraGameMode==nullptr) return;

	UCharacterClassInfo* CharacterClassInfo = GetCharacterInfo(WorldContextObject);	//获取游戏模式中的ClassInfo
	if(CharacterClassInfo==nullptr) return;
	for(TSubclassOf<UGameplayAbility> Ability : CharacterClassInfo->CommonAbilities)	//遍历ClassInfo中储存的敌人的技能数组
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Ability,1.f);
		ASC->GiveAbility(AbilitySpec);				//将技能逐个赋予该敌人的能力组件（受击等GA）
	}
	//敌人攻击相关的GA，储存在CharacterCLassInfo的FCharacterClassDefaultInfo中的数组中,在数据资产中进行添加.
	const FCharacterClassDefaultInfo CharacterClassDefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);
	for(TSubclassOf<UGameplayAbility> Ability : CharacterClassDefaultInfo.StartupAbilities)
	{
		if(ASC->GetAvatarActor()->Implements<UCombatInterface>())	//通过接口获取AI等级
		{
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Ability,ICombatInterface::Execute_GetPlayerLevel(ASC->GetAvatarActor()));
			ASC->GiveAbility(AbilitySpec);
		}
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

void UAuraAbilitySystemLibrary::GetLivePlayersInRadius(const UObject* WorldContext,
	TArray<AActor*>& OutOverlappingActors, const TArray<AActor*> ActorsToIgnore, float Radius,
	const FVector& SphereOrigin)
{
	FCollisionQueryParams SphereParams;
	SphereParams.AddIgnoredActors(ActorsToIgnore);	//添加忽略检测的Actor

	if(const UWorld* World = GEngine->GetWorldFromContextObject(WorldContext,EGetWorldErrorMode::LogAndReturnNull))//World获取失败的情况下返回空并输出Log
	{
		TArray<FOverlapResult> Overlaps;//输出参数，储存检测到的重叠组件
		World->OverlapMultiByObjectType(				//对象类型检测重叠
			Overlaps,SphereOrigin,FQuat::Identity,		//范围形状的中心、旋转
			FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects),		//检测对象类型的列表
			FCollisionShape::MakeSphere(Radius),SphereParams);		//创建检测区域的形状，有立方体，球体和胶囊体；用于追踪的额外参数，比如这里用到的忽略Actor
		for(FOverlapResult& Overlap : Overlaps)
		{
			if(Overlap.GetActor()->Implements<UCombatInterface>() && !ICombatInterface::Execute_IsDead(Overlap.GetActor()))
			{
				OutOverlappingActors.AddUnique(ICombatInterface::Execute_GetAvatar(Overlap.GetActor()));
			}
		}
	}
}

bool UAuraAbilitySystemLibrary::IsNotFriend(AActor* FirstActor, AActor* SecondActor)
{
	const bool bBothPlayer = FirstActor->ActorHasTag("Player")&&SecondActor->ActorHasTag("Player");
	const bool bBothEnemy = FirstActor->ActorHasTag("Enemy")&&SecondActor->ActorHasTag("Enemy");
	const bool bFriend = bBothEnemy||bBothPlayer;
	return !bFriend;
}

int32 UAuraAbilitySystemLibrary::GetXPRewardForClassAndLevel(const UObject* WorldContextObject,
	ECharacterClass CharacterClass, int32 CharacterLevel)
{
	UCharacterClassInfo* CharacterClassInfo = GetCharacterInfo(WorldContextObject);		//获取角色信息
	if(CharacterClassInfo == nullptr) return 0;

	const FCharacterClassDefaultInfo CharacterClassDefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);	//通过角色类型获取具体信息
	const float XPReward = CharacterClassDefaultInfo.XPReward.GetValueAtLevel(CharacterLevel);	//根据等级获取经验值

	return static_cast<int32>(XPReward);
}