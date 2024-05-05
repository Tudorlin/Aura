// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbility/AuraGameplayAbility.h"
#include "AuraSummonAbility.generated.h"

/**
 * 哥布林巫师召唤技能
 */
UCLASS()
class AURA_API UAuraSummonAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	TArray<FVector>GetSpawnLocation();		//召唤物位置

	UFUNCTION(BlueprintPure)
	TSubclassOf<APawn>GetRandomMinionClass();

	UPROPERTY(EditDefaultsOnly,Category="Summoning")
	TArray<TSubclassOf<APawn>> MinionClass;		//召唤物类型

	UPROPERTY(EditDefaultsOnly,Category="Summoning")
	int32 MinionNum = 5;			//召唤物数量

	UPROPERTY(EditDefaultsOnly,Category="Summoning")
	float MinSpawnDistance = 50.f;	//最近召唤距离

	UPROPERTY(EditDefaultsOnly,Category="Summoning")
	float MaxSpawnDistance = 250.f;	//最远召唤距离

	UPROPERTY(EditDefaultsOnly,Category="Summoning")
	float SpawnSpread = 90.f;			//召唤位置覆盖的角度范围
};
