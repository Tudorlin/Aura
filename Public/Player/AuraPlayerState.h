// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "AuraPlayerState.generated.h"

class ULevelInfo;
class UAttributeSet;
/**
 * 
 */
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerStateChanged,int32)

UCLASS()
class AURA_API AAuraPlayerState : public APlayerState,public IAbilitySystemInterface  
{
	GENERATED_BODY()
public:
	AAuraPlayerState();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet()const {return AttributeSet; }

	FOnPlayerStateChanged OnXPChangedDelegate;
	FOnPlayerStateChanged OnLevelChangedDelegate;

	FORCEINLINE int32 GetPlayerLevel() const { return Level; }
	FORCEINLINE int32 GetXP()const { return XP; }

	void AddToXP(int32 InXP);
	void AddToLevel(int32 InLevel);

	void SetXP(int32 InXP);
	void SetLevel(int32 InLevel);

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<ULevelInfo> LevelUpInfo;	//需要在外部访问，放公共部分

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;  //将组件设置在playerstate的好处：当角色死亡需要重生时销毁pawn的时候不会销毁playerstate，角色的属性可以得到保留，如果放置在角色身上状态的保存会复杂一些。

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

private:
	UPROPERTY(VisibleAnywhere,ReplicatedUsing=OnRep_Level)
	int32 Level  = 1;

	UFUNCTION()
	void OnRep_Level(int32 OldLevel);

	UPROPERTY(VisibleAnywhere,ReplicatedUsing=OnRep_XP)
	int32 XP = 0;

	UFUNCTION()
	void OnRep_XP(int32 OldXP);
};
