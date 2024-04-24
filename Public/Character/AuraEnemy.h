// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interfaction/EnemyInterface.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "AuraEnemy.generated.h"

class UWidgetComponent;
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
	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;
	//End IEnemyInterface End

	/********* CombatInterface Start ****************/
	virtual int32 GetPlayerLevel() override;
	/********* CombatInterface End ****************/

	UPROPERTY(BlueprintAssignable)
	FOnAttributesChangedSignature OnHealthChanged;    //注意这个委托定义在OverlayWidgetController类中

	UPROPERTY(BlueprintAssignable)
	FOnAttributesChangedSignature OnMaxHealthChanged;

protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Character Default Class")
	int32 Level = 1;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBar;
};
