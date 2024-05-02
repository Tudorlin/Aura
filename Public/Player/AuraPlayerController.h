// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/PlayerController.h"
#include "Input/AuraInputConfig.h"
#include "AuraPlayerController.generated.h"
class UDamageTextComponent;
class USplineComponent;
class UAuraAbilitySystemComponent;
class UInputMappingContext;
class UInputAction;
class IEnemyInterface;
struct FInputActionValue;
/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AAuraPlayerController();
	virtual void PlayerTick(float DeltaTime) override;   //与tick的区别在于playertick只会被本地控制时调用，而tick可以用于服务端和非本地的Controller

	UFUNCTION(Client,Reliable)		//函数在服务端调用，在客户端执行
	void ShowDamageText(float Damage,ACharacter* TargetCharacter,bool Blocked,bool bCriticalHit);
	
protected:
	virtual void BeginPlay() override;
	virtual  void SetupInputComponent() override;


private:
	UPROPERTY(EditAnywhere,Category="Input")
	TObjectPtr<UInputMappingContext>AuraContext;

	UPROPERTY(EditAnywhere,Category="Input")
	TObjectPtr<UInputAction>MoveAction;

	UPROPERTY(EditAnywhere,Category="Input")     //按下shift时即使没有选中敌人也能发动技能
	TObjectPtr<UInputAction>ShiftAction;

	void ShiftButtonPressed() { bShiftPressed = true; }
	void ShiftButtonReleased() { bShiftPressed = false; }
	
	bool bShiftPressed = false;
	
	void Move(const FInputActionValue& InputActionValue);

	void CursorTrace();  //光标的检测函数,在本地每帧调用

	IEnemyInterface* LastActor;   //TObjectPtr只有继承与UObject的类才能用，为什么突然不能用了还未知
	IEnemyInterface* ThisActor;
	
	FHitResult CursorHit;

	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);

	UPROPERTY(EditDefaultsOnly,Category="Input")
	TObjectPtr<UAuraInputConfig> InputConfig;

	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent;
	
	UAuraAbilitySystemComponent* GetASC();

	FVector CachedDestination = FVector::ZeroVector;  //目的地位置
	float FollowTime = 0.f;
	float ShortPressThreshold = 0.5f;  //最短触发时间，低于此值视为点击一次，超过则视为长按
	bool bAutoRunning = false;      //大概是判断是否要用简单移动
	bool bTargeting = false;      //鼠标是否有选中目标，没有目标时就是移动

	UPROPERTY(EditDefaultsOnly)
	float AutoRunAcceptRadius = 50.f;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent> Spline;

	void AutoRun();  //自动行走

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamageTextComponent> DamageTextComponentClass;
};
