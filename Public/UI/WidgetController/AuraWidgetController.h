// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "UObject/NoExportTypes.h"
#include "AuraWidgetController.generated.h"

class UAttributeSet;
class UAbilitySystemComponent;

USTRUCT(BlueprintType)    //创建结构体便于初始化控件控制器
struct FWidgetControllerParams
{
	GENERATED_BODY()
	FWidgetControllerParams() {};  //构造函数
	FWidgetControllerParams(APlayerController* PC,APlayerState* PS,UAbilitySystemComponent*ASC,UAttributeSet*AS) :
	PlayerController(PC),PlayerState(PS),AbilitySystemComponent(ASC),AttributeSet(AS){}

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TObjectPtr<APlayerController>PlayerController = nullptr;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TObjectPtr<APlayerState>PlayerState = nullptr;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent>AbilitySystemComponent = nullptr;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TObjectPtr<UAttributeSet>AttributeSet = nullptr;
};
/**
 * 
 */
UCLASS()
class AURA_API UAuraWidgetController : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void SetWidgetControllerParams(const FWidgetControllerParams& WCParams);  //初始化控制器中的变量

	virtual void BroadcastInitialValue();    //广播数值函数

	virtual void BindCallbacksDependencies();
	
protected:
	UPROPERTY(BlueprintReadOnly,Category="WidgetController")
	TObjectPtr<APlayerController>PlayerController;
	UPROPERTY(BlueprintReadOnly,Category="WidgetController")
	TObjectPtr<APlayerState>PlayerState;
	UPROPERTY(BlueprintReadOnly,Category="WidgetController")
	TObjectPtr<UAbilitySystemComponent>AbilitySystemComponent;
	UPROPERTY(BlueprintReadOnly,Category="WidgetController")
	TObjectPtr<UAttributeSet>AttributesSet;
};
