// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Ability/AuraFireBolt.h"

#include "AuraGameplayTags.h"

FString UAuraFireBolt::GetDescription(int32 Level)
{
	const int32 ScaleDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	if (Level == 1)
	{
		return FString::Printf(TEXT(
			// Title
			"<Title>火球</>\n\n"

			// Level
			"<Small>等级: </><Level>%d</>\n"
			// ManaCost
			"<Small>魔力消耗: </><ManaCost>%.1f</>\n"
			// Cooldown
			"<Small>冷却时间: </><Cooldown>%.1f</>\n\n"

			"<Default>发射一颗火球, "
			"在目标处爆炸并造成: </>"

			// Damage
			"<Damage>%d</><Default> 火焰伤害"
			" 并有概率使目标燃烧</>"),

			// Values
			Level,
			ManaCost,
			Cooldown,
			ScaleDamage);
	}
	else
	{
		return FString::Printf(TEXT(
			// Title
			"<Title>火球</>\n\n"

			// Level
			"<Small>等级: </><Level>%d</>\n"
			// ManaCost
			"<Small>魔力消耗: </><ManaCost>%.1f</>\n"
			// Cooldown
			"<Small>冷却时间: </><Cooldown>%.1f</>\n\n"

			// Number of FireBolts
			"<Default>发射 %d 颗火球, "
			"在目标处爆炸并造成: </>"

			// Damage
			"<Damage>%d</><Default> 火焰伤害"
			" 并有概率使目标燃烧</>"),

			// Values
			Level,
			ManaCost,
			Cooldown,
			FMath::Min(Level, NumProjectiles),
			ScaleDamage);
	}
}

FString UAuraFireBolt::GetNextLevelDescription(int32 Level)
{
	const int32 ScaleDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	return FString::Printf(TEXT(
			// Title
			"<Title>下一级: </>\n\n"

			// Level
			"<Small>等级: </><Level>%d</>\n"
			// ManaCost
			"<Small>魔力消耗: </><ManaCost>%.1f</>\n"
			// Cooldown
			"<Small>冷却时间: </><Cooldown>%.1f</>\n\n"

			// Number of FireBolts
			"<Default>发射 %d 颗火球, "
			"在目标处爆炸并造成: </>"

			// Damage
			"<Damage>%d</><Default> 火焰伤害"
			" 并有概率使目标燃烧</>"),

			// Values
			Level,
			ManaCost,
			Cooldown,
			FMath::Min(Level, NumProjectiles),
			ScaleDamage);
}

