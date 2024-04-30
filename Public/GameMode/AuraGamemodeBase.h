// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AuraGamemodeBase.generated.h"

class UCharacterClassInfo;
/**
 * 
 */
UCLASS()
class AURA_API AAuraGamemodeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly,Category="CharacterClassInfo")		//敌人属性初始化用的数据资产
	TObjectPtr<UCharacterClassInfo> CharacterClassInfo;
};
