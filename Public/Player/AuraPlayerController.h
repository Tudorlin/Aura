// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/PlayerController.h"
#include "AuraPlayerController.generated.h"
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
protected:
	virtual void BeginPlay() override;
	virtual void PlayerTick(float DeltaTime) override;   //与tick的区别在于playertick只会被本地控制时调用，而tick可以用于服务端和非本地的Controller
	virtual  void SetupInputComponent() override;


private:
	UPROPERTY(EditAnywhere,Category="Input")
	TObjectPtr<UInputMappingContext>AuraContext;

	UPROPERTY(EditAnywhere,Category="Input")
	TObjectPtr<UInputAction>MoveAction;

	void Move(const FInputActionValue& InputActionValue);

	void CursorTrace();  //光标的检测函数,在本地每帧调用

	IEnemyInterface* LastActor;   //TObjectPtr只有继承与UObject的类才能用，为什么突然不能用了还未知
	IEnemyInterface* ThisActor;
};
