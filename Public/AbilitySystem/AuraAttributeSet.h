// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectExtension.h"
#include "AuraAttributeSet.generated.h"

/**
 * 
 */  //此宏是一组属性访问函数，定义了GetAttribute（获取的是属性）,Get(获取的是浮点),Set和Init等操作，便于访问属性，避免手动实现这些操作
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

USTRUCT(BlueprintType)
struct FEffectProperties
{
	GENERATED_BODY()
	FEffectProperties(){};

	FGameplayEffectContextHandle EffectContextHandle;

	//源信息
	UPROPERTY()
	AActor* SourceAvatarActor = nullptr;
	UPROPERTY()
	ACharacter* SourceCharacter = nullptr;
	UPROPERTY()
	AController* SourceController = nullptr;
	UPROPERTY()
	UAbilitySystemComponent* SourceASC = nullptr;

	//目标信息
	UPROPERTY()
	AActor* TargetAvatarActor = nullptr;
	UPROPERTY()
	ACharacter* TargetCharacter = nullptr;
	UPROPERTY()
	AController* TargetController = nullptr;
	UPROPERTY()
	UAbilitySystemComponent* TargetASC = nullptr;
	
};
//typedef TBaseStaticDelegateInstance<FGameplayAttribute(), FDefaultDelegateUserPolicy>::FFuncPtr FAttributeFuncPtr;
template<class T>
using TStaticFunPtr = typename TBaseStaticDelegateInstance<T,FDefaultDelegateUserPolicy>::FFuncPtr;//TBaseStaticDelegateInstance，的作用是将静态函数绑定到委托上，这里作用是将对应的属性访问器与对应Tag关联
//举个例子：假设有一个FAttributeSignature 类型的委托 StrengthDelegate,它的功能就类似于实现: StrengthDelegate.AddStatic(UAuraAttributes::GetStrengthAttribute)

UCLASS()
class AURA_API UAuraAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	UAuraAttributeSet();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	TMap<FGameplayTag,TStaticFunPtr<FGameplayAttribute()>> TagToAttributes;       //本质是GameplayTag与委托的Map

	/*********主要属性值***********/
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing=OnRep_Strength,Category="Primary Attributes")     //力量
	FGameplayAttributeData Strength;
	UFUNCTION()
	void OnRep_Strength(const FGameplayAttributeData& OldStrength) const;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,Strength);

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing=OnRep_Intelligence,Category="Primary Attributes")   //智力
	FGameplayAttributeData Intelligence;
	UFUNCTION()
	void OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,Intelligence);

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing=OnRep_Resilience,Category="Primary Attributes")  //韧性
	FGameplayAttributeData Resilience;
	UFUNCTION()
	void OnRep_Resilience(const FGameplayAttributeData& OldResilience) const;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,Resilience) ; 

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing=OnRep_Vigor,Category="Primary Attributes") //精力
	FGameplayAttributeData Vigor;
	UFUNCTION()
	void OnRep_Vigor(const FGameplayAttributeData& OldVigor) const;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,Vigor);
	/*********主要属性值***********/


	/************次要属性,设定为根据主要属性的值进行能力成长***********************/

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing=OnRep_Armor,Category="Secondary Attributes")   //护甲值，基于韧性
	FGameplayAttributeData Armor;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,Armor);
	UFUNCTION()
	void OnRep_Armor(const FGameplayAttributeData& OldArmor) const;

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing=OnRep_ArmorPenetration,Category="Secondary Attributes")  //护甲穿透，基于韧性
	FGameplayAttributeData ArmorPenetration;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,ArmorPenetration);
	UFUNCTION()
	void OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration) const;  //格挡几率，基于护甲值

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing=OnRep_BlockChance,Category="Secondary Attributes")
	FGameplayAttributeData BlockChance;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,BlockChance);
	UFUNCTION()
	void OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const;

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing=OnRep_CriticalHitChance,Category="Secondary Attributes")  //暴击率，基于护甲穿透
	FGameplayAttributeData CriticalHitChance;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,CriticalHitChance);
	UFUNCTION()
	void OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance) const;

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing=OnRep_CriticalHitDamage,Category="Secondary Attributes")  //暴击伤害，基于护甲穿透
	FGameplayAttributeData CriticalHitDamage;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,CriticalHitDamage);
	UFUNCTION()
	void OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage) const;
	
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing=OnRep_CriticalHitResistance,Category="Secondary Attributes")  //暴击抗性，基于护甲值
	FGameplayAttributeData CriticalHitResistance;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,CriticalHitResistance);
	UFUNCTION()
	void OnRep_CriticalHitResistance(const FGameplayAttributeData& OldCriticalHitResistance) const;

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing=OnRep_HealthRegeneration,Category="Secondary Attributes")  //生命恢复，基于活力
	FGameplayAttributeData HealthRegeneration;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,HealthRegeneration);
	UFUNCTION()
	void OnRep_HealthRegeneration(const FGameplayAttributeData& OldHealthRegeneration) const;
	
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing=OnRep_ManaRegeneration,Category="Secondary Attributes")  //魔力回复，基于智力
	FGameplayAttributeData ManaRegeneration;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,ManaRegeneration);
	UFUNCTION()
	void OnRep_ManaRegeneration(const FGameplayAttributeData& OldManaRegeneration) const;

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing=OnRep_MaxHealth,Category="Secondary Attributes")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,MaxHealth);
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const;

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing=OnRep_MaxMana,Category="Secondary Attributes")
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,MaxMana);
	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const;
	
	/************次要属性***********************/
	
	/********重要属性值*********/
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing=OnRep_Health,Category="Vital Attributes")
	FGameplayAttributeData Health;   //FGameplayAttributeData包括两个浮点值BaseValue和CurrentValue,前者为基础属性，通常包括永久性的变化；后者表示临时性的增益或者减益
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,Health);  //属性访问器访问Health
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth) const;

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing=OnRep_Mana,Category="Vital Attributes")
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,Mana);
	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldMana) const;
	/*******重要属性值********/

	/**********抗性*************/
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing=OnRep_FireResistance,Category="Resistance")
	FGameplayAttributeData FireResistance;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,FireResistance);
	UFUNCTION()
	void OnRep_FireResistance(const FGameplayAttributeData& OldFireResistance) const;

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing=OnRep_LightningResistance,Category="Resistance")
	FGameplayAttributeData LightningResistance;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,LightningResistance);
	UFUNCTION()
	void OnRep_LightningResistance(const FGameplayAttributeData& OldLightningResistance) const;

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing=OnRep_PhysicsResistance,Category="Resistance")
	FGameplayAttributeData PhysicalResistance;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,PhysicalResistance);
	UFUNCTION()
	void OnRep_PhysicsResistance(const FGameplayAttributeData& OldPhysicsResistance) const;

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing=OnRep_ArcaneResistance,Category="Resistance")
	FGameplayAttributeData ArcaneResistance;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,ArcaneResistance);
	UFUNCTION()
	void OnRep_ArcaneResistance(const FGameplayAttributeData& OldArcaneResistance) const;
	
	/**************元属性*************/
	UPROPERTY(BlueprintReadOnly,Category="Meta Attributes")
	FGameplayAttributeData IncomingDamage;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,IncomingDamage);

	UPROPERTY(BlueprintReadOnly, Category = "Meta Attributes")
	FGameplayAttributeData IncomingXP;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, IncomingXP);

    
private:
	void SetEffectProperties(const FGameplayEffectModCallbackData& Data,FEffectProperties& Props) const;
	void ShowFloatingText(const FEffectProperties& Props,float Damage,bool bBlocked,bool bCriticalHit) const;
	void SendXPEvent(const FEffectProperties& Props);		//发送事件到监听GA中接受
};
