// Fill out your copyright notice in the Description page of Project Settings.


#include "..\..\Public\Player\AuraPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include"EnhancedInputComponent.h"
#include "Interfaction/EnemyInterface.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;//属性复制，用于多人游戏将服务器实体发生的改变告知其他客户端
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(AuraContext);
	
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if(Subsystem)
		Subsystem->AddMappingContext(AuraContext,0);//0为输入优先级，防止多个输入映射发生冲突
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;  //鼠标样式，引擎内置了一般的鼠标样式，可以在UI中添加贴图修改

	FInputModeGameAndUI InputModeDate;
	InputModeDate.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);  //不会将鼠标锁定在视口内
	InputModeDate.SetHideCursorDuringCapture(false);    //鼠标即使是在按下时仍然可见
	SetInputMode(InputModeDate);     //设置输入模式
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	CursorTrace();
}

void AAuraPlayerController::CursorTrace()
{
	FHitResult CursorHit;
	GetHitResultUnderCursor(ECC_Visibility,false,CursorHit); //获取光标下的碰撞结果，储存在CursorHit中，由于只是简单碰撞所以性能消耗并不大
	if(!CursorHit.bBlockingHit) return;

	LastActor = ThisActor;
	ThisActor = Cast<IEnemyInterface>(CursorHit.GetActor());

	if(LastActor==nullptr)
	{
		if(ThisActor)
			ThisActor->HighLightActor();   //鼠标点击的上一个actor为空，当前有效，高亮当前即可，如果两个都无效则无事发生
	}
	else
	{
		if(ThisActor==nullptr)
			LastActor->UnHighLightActor();  //如果当前没检测到actor，则将上一个检测到的actor的高光去掉
		else
		{
			if(LastActor!=ThisActor)   //两个都有效时判断是不是同一个actor，如果不是则取消上一个的高亮并高亮当前actor，反之则无事发生。
			{
				LastActor->UnHighLightActor();
				ThisActor->HighLightActor();
			}
		}
	}
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);//castcheck会自动检查值的有效性，无效会崩溃，将inputcomponent投射到增强输入组件
	EnhancedInputComponent->BindAction(MoveAction,ETriggerEvent::Triggered,this,&AAuraPlayerController::Move);//传入moveaction,触发事件，目标（控制器本身），绑定的函数
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();   //2d和2D还有所不同，在使用ue的情况下通常用2D,是ue自带的结构体变量
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f,Rotation.Yaw,0.f); //定义yaw方向的旋转值

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);  //X轴方向的单位向量作前进方向,ue使用的是左手系
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);   //Y轴单位向量作向右方向

	if(APawn* ControllerPawn = GetPawn())
	{
		ControllerPawn->AddMovementInput(ForwardDirection,InputAxisVector.Y);   //输入绑定
		ControllerPawn->AddMovementInput(RightDirection,InputAxisVector.X);
	}
}


