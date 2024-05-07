// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Data/LevelInfo.h"

int32 ULevelInfo::FindLevelForXP(int32 XP) const
{
	int Level = 1;
	bool bSearching = true;
	while(bSearching)
	{
		if(LevelUpInformation.Num()-1<=Level) return Level;	//已经达到设定的最大等级
		if(XP>=LevelUpInformation[Level].LevelRequirement)	//查找当前经验可以到达的等级
		{
			Level++;
		}
		else
		{
			bSearching = false;
		}
	}
	return Level;
}
