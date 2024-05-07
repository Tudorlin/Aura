// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "OverlayWidgetController.generated.h"

struct FAuraAbilityInfo;
class UAuraAbilitySystemComponent;
class UAbilityInfo;
class UAuraUserWidget;

USTRUCT(BlueprintType)
struct FUIWidgetRow : public FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FGameplayTag MessageTag = FGameplayTag();

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FText Message = FText();

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TSubclassOf<UAuraUserWidget> MessageWidget;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	UTexture2D* Image = nullptr;
};

//定义动态多播委托

//不需要对每一个属性都定义一个委托，如果这样做的话随着属性的增多代码的可读性会非常差，定义一个即可。
// DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChangedSignature,float,NewHealth);
// DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxHealthChangedSignature,float,NewMaxHealth);
// DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnManaChangedSignature,float,NewMana);
// DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxManaChangedSignature,float,NewMaxMana);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerStateChangedSignature,int32,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributesChangedSignature,float,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMessageWidgetRowSignature,FUIWidgetRow,Row);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityInfoSignature,const FAuraAbilityInfo&,Info);	//广播技能信息


/**
 * 
 */
UCLASS(BlueprintType,Blueprintable)
class AURA_API UOverlayWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadcastInitialValue() override;
	virtual void BindCallbacksDependencies() override;   //绑定回调函数

	UPROPERTY(BlueprintAssignable,Category="GAS|Attributes")  //此宏表示可以用于多播委托
	FOnAttributesChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable,Category="GAS|Attributes")
	FOnAttributesChangedSignature OnMaxHealthChanged;

	UPROPERTY(BlueprintAssignable,Category="GAS|Attributes")
	FOnAttributesChangedSignature OnManaChanged;

	UPROPERTY(BlueprintAssignable,Category="GAS|Attributes")
	FOnAttributesChangedSignature OnMaxManaChanged;

	UPROPERTY(BlueprintAssignable,Category="GAS|Message")
	FMessageWidgetRowSignature MessageWidgetRowDelegate;

	UPROPERTY(BlueprintAssignable,Category="GAS|Message")		//返回技能图标这类信息
	FAbilityInfoSignature AbilityInfoDelegate;

	UPROPERTY(BlueprintAssignable,Category="GAS|Message")		//返回经验百分比
	FOnAttributesChangedSignature OnXPPercentChangedDelegate;

	UPROPERTY(BlueprintAssignable,Category="GAS|Message")
	FOnPlayerStateChangedSignature OnPlayerLevelChangedDelegate;

protected:
	//简短函数用Lambda表达式代替
	// void HealthChanged(const FOnAttributeChangeData& Data) const;//FOnAttributeChangeData是一个临时参数结构体，用于在属性发生变化时传递相关数据。
	// /**这个结构体包含以下成员变量：Attribute：表示发生变化的属性。GEModData：暂时未知。NewValue：表示属性的新值。OldValue：表示属性的旧值。**/
	// void MaxHealthChanged(const FOnAttributeChangeData& Data) const;
	// void ManaChanged(const FOnAttributeChangeData& Data) const;
	// void MaxManaChanged(const FOnAttributeChangeData& Data) const;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Widget Data")
	TObjectPtr<UDataTable> MessageWidgetDataTable;     //数据资产

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Wodget Data")
	TObjectPtr<UAbilityInfo> AbilityInfo;

	template<typename T>
	T* GetDataTableRowByTag(UDataTable* DataTable,const FGameplayTag Tag);  //根据tag获取数据表的行，使用模板可以返回任意类型的行

	void OnInitializeStartupAbilities(UAuraAbilitySystemComponent* AuraAbilitySystemComponent);		//判断玩家技能初始化是否完成

	void OnXPChanged(int32 NewXP) const;
};

template <typename T>
T* UOverlayWidgetController::GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag Tag)
{
	return DataTable->FindRow<T>(Tag.GetTagName(),TEXT(""));
}
