// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "AuraPlayerState.generated.h"

class UAttributeSet;
/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerState : public APlayerState,public IAbilitySystemInterface  
{
	GENERATED_BODY()
public:
	AAuraPlayerState();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet()const {return AttributeSet; }

	

protected:
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;  //将组件设置在playerstate的好处：当角色死亡需要重生时销毁pawn的时候不会销毁playerstate，角色的属性可以得到保留，如果放置在角色身上状态的保存会复杂一些。

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;
};
