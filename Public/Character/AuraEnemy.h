// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Character/AuraCharacterBase.h"
#include "Interfaction/EnemyInterface.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "AuraEnemy.generated.h"

class AAuraAIController;
class UBehaviorTree;
enum class ECharacterClass : uint8;
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
	virtual void PossessedBy(AController* NewController) override;
	
	// IEnemyInterface Start
	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;

	virtual void SetCombatTarget_Implementation(AActor* ICombatTarget) override;		//设置战斗目标，具体实现在蓝图中
	virtual  AActor* GetCombatTarget_Implementation() const override;
	//End IEnemyInterface End

	/********* CombatInterface Start ****************/
	virtual int32 GetPlayerLevel() override;
	virtual void Die() override;
	/********* CombatInterface End ****************/

	UPROPERTY(BlueprintAssignable)
	FOnAttributesChangedSignature OnHealthChanged;    //注意这个委托定义在OverlayWidgetController类中

	UPROPERTY(BlueprintAssignable)
	FOnAttributesChangedSignature OnMaxHealthChanged;

	void HitReactTagChanged(const FGameplayTag CallbackTag,int32 NewCount);

	UPROPERTY(BlueprintReadOnly,Category="Combat")
	bool bHitReacting = false;

	UPROPERTY(EditAnywhere,Category="Combat")
	float BaseWalkSpeed = 250.f;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Combat")
	float LifeSpan = 5.f;				//死亡后尸体生命周期

protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;
	virtual void InitializeDefaultAttributes() const override;		//重写敌人属性集初始化函数

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Character Default Class")
	int32 Level = 1;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBar;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Character Default Class")  //敌人类型
	ECharacterClass CharacterClass = ECharacterClass::Warrior;

	UPROPERTY(EditAnywhere,Category="AI")
	TObjectPtr<UBehaviorTree>BehaviorTree;

	UPROPERTY()
	TObjectPtr<AAuraAIController>	AuraAIController;

	UPROPERTY(BlueprintReadWrite,Category="Combat")
	TObjectPtr<AActor>CombatTarget;
};
