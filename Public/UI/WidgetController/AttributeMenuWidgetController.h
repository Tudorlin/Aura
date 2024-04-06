// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "AttributeMenuWidgetController.generated.h"

class UAttributeInfo;
struct FAuraAttributeInfo;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttributeInfoSignaure, const FAuraAttributeInfo&,Info);

/**
 * 
 */
UCLASS(BlueprintType,Blueprintable)
class AURA_API UAttributeMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
public:
	virtual void BroadcastInitialValue() override;    //广播操作做在属性菜单控制器设置完毕之后，作初始化。
	virtual void BindCallbacksDependencies() override;

	UPROPERTY(BlueprintAssignable, Category="Attributes")
	FAttributeInfoSignaure AttributeInfoDelegate;

protected:
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UAttributeInfo> AttributeInfo;   //包含Tag信息的那个数组中的类

private:
	void BroadcastAttributeInfo(const FGameplayTag& AttributeTag,const FGameplayAttribute&Attribute) const;
};
