// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

/**
 *AuraGameplayTags
 *
 * 包含本地游戏标签的单例
 * 单例是设计模式的一种；所谓单例，即整个类系统中只有一个实例，并提供一个全局的访问点用于访问该实例
 */
struct FAuraGameplayTags
{
public:
	static const FAuraGameplayTags& Get() { return GameplayTags; }    //访问点
	static void InitializeNativeGameplayTags();

	//主要属性tag
	FGameplayTag Attributes_Primary_Strength;
	FGameplayTag Attributes_Primary_Intelligence;
	FGameplayTag Attributes_Primary_Resilience;
	FGameplayTag Attributes_Primary_Vigor;

	//次要属性tag
	FGameplayTag Attributes_Secondary_Armor;
	FGameplayTag Attributes_Secondary_ArmorPenetration;
	FGameplayTag Attributes_Secondary_BlockChance;
	FGameplayTag Attributes_Secondary_CriticalHitChance;
	FGameplayTag Attributes_Secondary_CriticalHitDamage;
	FGameplayTag Attributes_Secondary_CriticalHitResistance;
	FGameplayTag Attributes_Secondary_HealthRegeneration;
	FGameplayTag Attributes_Secondary_ManaRegeneration;
	FGameplayTag Attributes_Secondary_MaxHealth;
	FGameplayTag Attributes_Secondary_MaxMana;

	//元属性
	FGameplayTag Attributes_Meta_IncomingXP;


	//输入tag
	FGameplayTag InputTag_LMB;
	FGameplayTag InputTag_RMB;
	FGameplayTag Input_1;
	FGameplayTag Input_2;
	FGameplayTag Input_3;
	FGameplayTag Input_4;
	FGameplayTag Input_Passive_1;
	FGameplayTag Input_Passive_2;


	//伤害类型
	FGameplayTag Damage;
	FGameplayTag Damage_Fire;	//火焰伤害
	FGameplayTag Damage_Lightning;		//雷电伤害
	FGameplayTag Damage_Arcane; //魔法伤害
	FGameplayTag Damage_Physics;	//物理伤害

	//抗性
	FGameplayTag Attributes_Resistance_Fire;	//抗性
	FGameplayTag Attributes_Resistance_Lightning;
	FGameplayTag Attributes_Resistance_Arcane;
	FGameplayTag Attributes_Resistance_Physical;

	//不同类型对应的Debuff
	FGameplayTag Debuff_Burn;
	FGameplayTag Debuff_Stun;
	FGameplayTag Debuff_Arcane;
	FGameplayTag Debuff_Physical;

	//debuff相关的参数
	FGameplayTag Debuff_Chance;
	FGameplayTag Debuff_Damage;
	FGameplayTag Debuff_Duration;
	FGameplayTag Debuff_Frequency;
	

	
	FGameplayTag Abilities_None;
	FGameplayTag Abilities_Attack;		//攻击
	FGameplayTag Abilities_Summon;		//召唤
	

	//技能状态相关:
	FGameplayTag Abilities_HitReact;	//角色受击
	
	FGameplayTag Abilities_Status_Locked;
	FGameplayTag Abilities_Status_Eligible;
	FGameplayTag Abilities_Status_Unlocked;
	FGameplayTag Abilities_Status_Equipped;

	FGameplayTag Abilities_Type_Offensive;
	FGameplayTag Abilities_Type_Passive;
	FGameplayTag Abilities_Type_None;

	//技能图标相关Tag
	FGameplayTag Abilities_Fire_FireBlot;
	FGameplayTag Abilities_Lightning_Electrocute;

	//技能冷却Tag
	FGameplayTag Abilities_Cooldown_FireBlot;

	//CombatSocket
	FGameplayTag CombatSocket_Weapon;
	FGameplayTag CombatSocket_LeftHand;
	FGameplayTag CombatSocket_RightHand;
	FGameplayTag CombatSocket_Tail;

	//蒙太奇,将插槽Tag与蒙太奇Tag区分开，同时也用于攻击时应当播放的音效，在GameplayCue中使用
	FGameplayTag Montage_Attack_1;
	FGameplayTag Montage_Attack_2;
	FGameplayTag Montage_Attack_3;
	FGameplayTag Montage_Attack_4;

	TMap<FGameplayTag,FGameplayTag> DamageTypesToResistances;	//将各种类型的伤害与抗性对应
	TMap<FGameplayTag,FGameplayTag> DamageTypesToDebuffs;		//将各种类型的伤害与debuff对应

	FGameplayTag Effect_HitReact;   //受击时的反应
protected:

private:
	static FAuraGameplayTags GameplayTags;
};
