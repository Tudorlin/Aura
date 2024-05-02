// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "DamageTextComponent.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UDamageTextComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable)
	void SetDamage(float Damage,bool bBlocked,bool bCriticalHit);			//调用路径：属性集中通过IncomingDamage计算伤害时调用成员函数ShowFloatingText,在这个函数中调用PC中的ShowDamageText函数，然后会调用此函数
};
