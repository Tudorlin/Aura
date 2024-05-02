// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AuraAbilitySystemLibrary.generated.h"

class UCharacterClassInfo;
class UAbilitySystemComponent;
enum class ECharacterClass : uint8;
class UAttributeMenuWidgetController;
class UOverlayWidgetController;
/**
 * 
 */
UCLASS()
class AURA_API UAuraAbilitySystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()


public:
	UFUNCTION(BlueprintPure,Category="WidgetController")   //此宏修饰的函数没有执行引脚
	static UOverlayWidgetController* GetOverlayWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure,Category="WidgetController")
	static UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable,Category="AuraAbilitySystemLibrary | CharacterClassDefault")		//初始化敌人的属性，角色的初始化在CharacterBase中
	static void InitializeDefaultAttributes(const UObject* WorldContextObject,ECharacterClass CharacterClass,float Level,UAbilitySystemComponent* ASC);

	UFUNCTION(BlueprintCallable,Category="AuraAbilitySystemLibrary | CharacterClassDefault")
	static void GiveStartupAbilities(const UObject* WorldContextObject,UAbilitySystemComponent* ASC);	//为敌人添加技能

	UFUNCTION(BlueprintCallable,Category="AuraAbilitySystemLibrary | CharacterClassDefault")		//获取游戏模式中的CharacterClassInfo
	static UCharacterClassInfo* GetCharacterInfo(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure,Category="AuraAbilitySystemLibrary | GameplayEffefts")			//自定义Context中的属性设置与获取
	static bool IsBlocked(const FGameplayEffectContextHandle& ContextHandle);

	UFUNCTION(BlueprintPure,Category="AuraAbilitySystemLibrary | GameplayEffefts")
	static bool IsCriticalHit(const FGameplayEffectContextHandle& ContextHandle);

	UFUNCTION(BlueprintCallable,Category="AuraAbilitySystemLibrary | GameplayEffefts")
	static void SetIsBlocked(UPARAM(ref) FGameplayEffectContextHandle& ContextHandle,bool bInIsBlocked);			//计算伤害时判定并设置
	//如果输入参数是非常量引用，引擎会判断此函数是输出而不是输入，为了让它变成输入，需要用UPARAM(ref)签名标识

	UFUNCTION(BlueprintCallable,Category="AuraAbilitySystemLibrary | GameplayEffefts")
	static void SetIsCriticalHit(UPARAM(ref) FGameplayEffectContextHandle& ContextHandle, bool bInIsCriticalHit);
	
};
