// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interfaction/EnemyInterface.h"
#include "AuraEnemy.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase,public IEnemyInterface
{
	GENERATED_BODY()
public:
	AAuraEnemy();
	// IEnemyInterface Start
	virtual void HighLightActor() override;
	virtual void UnHighLightActor() override;
	//End IEnemyInterface End

	/********* CombatInterface Start ****************/
	virtual int32 GetPlayerLevel() override;
	/********* CombatInterface End ****************/

protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Character Default Class")
	int32 Level = 1;
};
