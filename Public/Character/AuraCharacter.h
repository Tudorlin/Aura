// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interfaction/PlayerInterface.h"
#include "NiagaraComponent.h"
#include "AuraCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
/**
 * 
 */
UCLASS()
class AURA_API AAuraCharacter : public AAuraCharacterBase,public IPlayerInterface
{
	GENERATED_BODY()
public:
	AAuraCharacter();

	virtual void PossessedBy(AController* NewController) override;  //在服务端上初始化
	virtual void OnRep_PlayerState() override;		//在客户端上初始化

protected:
	virtual  void InitAbilityActorInfo() override;    //初始化能力组件和属性集

	/********* CombatInterface Start ****************/
	virtual int32 GetPlayerLevel_Implementation() override;
	/********* CombatInterface End ****************/

	/**********PlayerInterface Start************/			//这一堆函数从PlayerState中获取需要的信息
	virtual int32 FindLevelForXP_Implementation(int32 InXP) const override;
	
	virtual int32 GetXP_Implementation() const override;		//经验
	virtual void AddToXP_Implementation(int32 InXP) override;

	virtual int32 GetAttributePointsReward_Implementation(int32 Level) const override;	//属性点
	virtual void AddToAttributePoints_Implementation(int32 InAttributePoints) override;

	virtual int32 GetSpellPointsReward_Implementation(int32 Level) const override;	//技能点
	virtual void AddToSpellPoints_Implementation(int32 InSpellPoints) override;

	virtual void LevelUp_Implementation() override;		//升级
	virtual void AddToPlayerLevel_Implementation(int32 InPlayerLevel) override;
	virtual int32 GetAttributePoints_Implementation() const override;
	virtual int32 GetSpellPoints_Implementation() const override;
	/**********PlayerInterface End************/

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> LevelUpNiagaraComponent;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> TopDownCameraComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> CameraBoom;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastLevelUpParticles() const;
};
