// Fill out your copyright notice in the Description page of Project Settings.

#include "GameplayAbility/AuraGameplayAbility.h"

FString UAuraGameplayAbility::GetDescription(int32 Level)
{
	return FString::Printf(TEXT("<Default>%s, </><Level>%d</>"),
		L"Default Ability Name - LoremIpsum LoremIpsum LoremIpsum "
  "LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum "
  "LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum", Level);
}

FString UAuraGameplayAbility::GetNextLevelDescription(int32 Level)
{
	return FString::Printf(TEXT("<Default>下一级: </><Level>%d</> "
							 "\n<Default>造成更多伤害。 </>"), Level);
}

FString UAuraGameplayAbility::GetLockedDescription(int32 Level)
{
	return FString::Printf(TEXT("<Default>最低等级要求: %d</>"), Level);
}

float UAuraGameplayAbility::GetManaCost(float InLevel) const
{
	float ManaCost = 0.f;
	if (const UGameplayEffect* CostEffect = GetCostGameplayEffect())	//获取cost的GE
	{
		for (FGameplayModifierInfo Mod : CostEffect->Modifiers)		//获取修改器中的信息
		{
			if (Mod.Attribute == UAuraAttributeSet::GetManaAttribute())	//定位魔法值消耗
			{
				Mod.ModifierMagnitude.GetStaticMagnitudeIfPossible(InLevel, ManaCost);//获取消耗值
				break;
			}
		}
	}
	return ManaCost;
}

float UAuraGameplayAbility::GetCooldown(float InLevel) const
{
	float Cooldown = 0.f;
	if (const UGameplayEffect* CooldownEffect = GetCooldownGameplayEffect())
	{
		CooldownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(InLevel, Cooldown);	//获取冷却GE的持续时间
	}
	return Cooldown;
}
