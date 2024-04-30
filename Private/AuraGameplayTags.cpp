// Fill out your copyright notice in the Description page of Project Settings.


#include "AuraGameplayTags.h"
#include "GameplayTagsManager.h"

FAuraGameplayTags FAuraGameplayTags::GameplayTags;    //唯一实例

void FAuraGameplayTags::InitializeNativeGameplayTags()
{
	//用tag管理器添加标签:

	//主要属性:
	GameplayTags.Attributes_Primary_Strength = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Strength"),FString("力量，增加物理伤害"));
	
	GameplayTags.Attributes_Primary_Intelligence = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Intelligence"),FString("智力，增加魔法伤害"));
	
	GameplayTags.Attributes_Primary_Resilience = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Resilience"),FString("韧性，增加护甲"));
	
	GameplayTags.Attributes_Primary_Vigor = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Vigor"),FString("活力，增加回复及生命"));


	
	//次要属性
	GameplayTags.Attributes_Secondary_Armor = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.Armor"),FString("护甲，减少受到的伤害"));
		
	GameplayTags.Attributes_Secondary_ArmorPenetration = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.ArmorPenetration"),FString("护甲穿透，增加暴击率"));
	
	GameplayTags.Attributes_Secondary_BlockChance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.BlockChance"),FString("受到伤害减半的几率"));
	
	GameplayTags.Attributes_Secondary_CriticalHitChance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.CriticalHitChance"),FString("暴击率"));
	
	GameplayTags.Attributes_Secondary_CriticalHitDamage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.CriticalHitDamage"),FString("暴击伤害"));
	
	GameplayTags.Attributes_Secondary_CriticalHitResistance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.CriticalHitResistance"),FString("暴击抗性"));
	
	GameplayTags.Attributes_Secondary_HealthRegeneration = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.HealthRegeneration"),FString("生命回复"));
	
	GameplayTags.Attributes_Secondary_ManaRegeneration = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.ManaRegeneration"),FString("魔力回复"));
	
	GameplayTags.Attributes_Secondary_MaxHealth = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.MaxHealth"),FString("生命最大值"));
	
	GameplayTags.Attributes_Secondary_MaxMana = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.MaxMana"),FString("魔法最大值"));

	//输入Tag
	GameplayTags.InputTag_LMB = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Input.LMB"),FString("鼠标左键"));
	
	GameplayTags.InputTag_RMB = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Input.RMB"),FString("鼠标右键"));
	
	GameplayTags.Input_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Input.1"),FString("数字键1"));
	
	GameplayTags.Input_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Input.2"),FString("数字键2"));
	
	GameplayTags.Input_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Input.3"),FString("数字键3"));
	
	GameplayTags.Input_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Input.4"),FString("数字键4"));


	GameplayTags.Damage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage"),FString("伤害"));

	GameplayTags.Effect_HitReact = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Effect.HitReact"),FString("受击Tag"));
}
