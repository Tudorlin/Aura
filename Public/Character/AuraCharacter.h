// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "AuraCharacter.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraCharacter : public AAuraCharacterBase
{
	GENERATED_BODY()
public:
	AAuraCharacter();

	virtual void PossessedBy(AController* NewController) override;  //在服务端上初始化
	virtual void OnRep_PlayerState() override;		//在客户端上初始化

protected:
	virtual  void InitAbilityActorInfo() override;    //初始化能力组件和属性集

	/********* CombatInterface Start ****************/
	virtual int32 GetPlayerLevel() override;
	/********* CombatInterface End ****************/
	

private:

};
