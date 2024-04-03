// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/AuraHUD.h"

#include "UI/Widget/AuraUserWidget.h"
#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "UI/WidgetController/OverlayWidgetController.h"

UOverlayWidgetController* AAuraHUD::GetOverlayWidgetController(const FWidgetControllerParams& WCParams)
{
	if(OverlayWidgetController==nullptr)
	{
		OverlayWidgetController = NewObject<UOverlayWidgetController>(this,OverlayWidgetControllerClass);  //控制器为空则以传入的蓝图类创建新对象
		OverlayWidgetController->SetWidgetControllerParams(WCParams);  //初始化HUD中控制器变量
		OverlayWidgetController->BindCallbacksDependencies();
		return OverlayWidgetController;
	}
	return OverlayWidgetController;
}

UAttributeMenuWidgetController* AAuraHUD::GetAttributeMenuWidgetController(const FWidgetControllerParams& WCParams)
{
	if(AttributeMenuWidgetController==nullptr)
	{
		AttributeMenuWidgetController = NewObject<UAttributeMenuWidgetController>(this,AttributeMenuWidgetControllerClass);
		AttributeMenuWidgetController->SetWidgetControllerParams(WCParams);
		AttributeMenuWidgetController->BindCallbacksDependencies();
	}
	return AttributeMenuWidgetController;
}

void AAuraHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{

	//检查蓝图中是否有赋值
	checkf(OverlayWidgetClass,TEXT("OverlayWidgetClass is uninitialized,Please fill out BP_OverlayWidget"));
	checkf(OverlayWidgetControllerClass,TEXT("OverlayWidgetControllerClass is uninitialized,please fill out BP_OverlayWidgetController"))
	
	UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), OverlayWidgetClass); //暂存传入的蓝图类
	OverlayWidget = Cast<UAuraUserWidget>(Widget);   //转化为OverlayWidget进行初始化操作
	const FWidgetControllerParams WidgetControllerParams(PC,PS,ASC,AS);  //创建结构体作参数
	UOverlayWidgetController* WidgetController = GetOverlayWidgetController(WidgetControllerParams);    //调用AuraWidgetController类中的初始化函数
	OverlayWidget->SetWidgetController(WidgetController);  //调用AuraUserWidget类中的函数将控制器设置为用户控件的控制器

	WidgetController->BroadcastInitialValue();  //控制器设置完成才开始广播
	Widget->AddToViewport();
}

