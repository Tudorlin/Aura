// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AuraUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraUserWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable) //可以在蓝图中调用
	void SetWidgetController(UObject* InWidgetController);
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UObject>WidgetController;    //便于管理属性和代码重用

protected:
	UFUNCTION(BlueprintImplementableEvent)  //此宏作用为c++中声明函数，在蓝图中实现
	void WidgetControllerSet();
};
