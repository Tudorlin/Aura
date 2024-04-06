// Fill out your copyright notice in the Description page of Project Settings.


#include "..\..\Public\Player\AuraPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "EnhancedInputSubsystems.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Components/SplineComponent.h"
#include "Input/AuraInputComponent.h"
#include "Interfaction/EnemyInterface.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;//属性复制，用于多人游戏将服务器实体发生的改变告知其他客户端
	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
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
	AutoRun();
}

void AAuraPlayerController::CursorTrace()
{
	GetHitResultUnderCursor(ECC_Visibility,false,CursorHit); //获取光标下的碰撞结果，储存在CursorHit中，由于只是简单碰撞所以性能消耗并不大
	if(!CursorHit.bBlockingHit) return;

	LastActor = ThisActor;
	ThisActor = Cast<IEnemyInterface>(CursorHit.GetActor());

	// if(LastActor==nullptr)
	// {
	// 	if(ThisActor)
	// 		ThisActor->HighLightActor();   //鼠标点击的上一个actor为空，当前有效，高亮当前即可，如果两个都无效则无事发生
	// }
	// else
	// {
	// 	if(ThisActor==nullptr)
	// 		LastActor->UnHighLightActor();  //如果当前没检测到actor，则将上一个检测到的actor的高光去掉
	// 	else
	// 	{
	// 		if(LastActor!=ThisActor)   //两个都有效时判断是不是同一个actor，如果不是则取消上一个的高亮并高亮当前actor，反之则无事发生。
	// 		{
	// 			LastActor->UnHighLightActor();
	// 			ThisActor->HighLightActor();
	// 		}
	// 	}
	// }    简化代码:
	if(LastActor!=ThisActor)
	{
		if(LastActor) LastActor->UnHighLightActor();
		if(ThisActor) ThisActor->HighLightActor();
	}
}

void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	//GEngine->AddOnScreenDebugMessage(1, 3.f, FColor::Red, *InputTag.ToString());
	if(InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		bTargeting = ThisActor ? true : false;
		bAutoRunning = false;
	}
}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	//GEngine->AddOnScreenDebugMessage(2, 3.f, FColor::Blue, *InputTag.ToString());
	//if(GetASC()==nullptr) return;
	//GetASC()->AbilityInputTagReleased(InputTag);
	if(!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		if(GetASC())
		{
			GetASC()->AbilityInputTagReleased(InputTag);
		}
		return;
	}
	if(bTargeting)
	{
		if(GetASC())
		{
			GetASC()->AbilityInputTagReleased(InputTag);
		}
	}												//作用同下
	else
	{
		APawn* ControlledPawn = GetPawn();
		if(FollowTime<=ShortPressThreshold)
		{
			if(UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(
				this,ControlledPawn->GetActorLocation(),CachedDestination))  //这里的目标位置在下面那个函数，即鼠标点击时设置了，获取从角色到目标点的路线
			{
				Spline->ClearSplinePoints();
				for(const FVector& Point : NavPath->PathPoints)
				{
					Spline->AddSplinePoint(Point,ESplineCoordinateSpace::World);
					//DrawDebugSphere(GetWorld(),Point,8.f,8,FColor::Red,false,5.f);
				}
				CachedDestination = NavPath->PathPoints[NavPath->PathPoints.Num()-1];   //选择最后一个点作为最终目标，防止角色乱跑的情况出现
				bAutoRunning = true;
			}
		}
		FollowTime = 0.f;
		bTargeting = false;
	}
}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	//GEngine->AddOnScreenDebugMessage(3, 3.f, FColor::Green, *InputTag.ToString());
	//if(GetASC()==nullptr) return;
	//GetASC()->AbilityInputTagHeld(InputTag);
	if(!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))  //除了鼠标左键的其他案件正常触发技能
	{
		if(GetASC())
		{
			GetASC()->AbilityInputTagHeld(InputTag);
		}
		return;
	}
	if(bTargeting)
	{
		if(GetASC())
		{
			GetASC()->AbilityInputTagHeld(InputTag);
		}
	}
	else
	{
		FollowTime +=GetWorld()->GetDeltaSeconds();
		
		if(CursorHit.bBlockingHit)
		{
			CachedDestination = CursorHit.ImpactPoint;
		}
		if(APawn* ControlledPawn = GetPawn())
		{
			const FVector WorldDirection = (CachedDestination-ControlledPawn->GetActorLocation()).GetSafeNormal();
			ControlledPawn->AddMovementInput(WorldDirection);
		}
	}
}

UAuraAbilitySystemComponent* AAuraPlayerController::GetASC()
{
	if(AuraAbilitySystemComponent==nullptr)
		AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	return AuraAbilitySystemComponent;
}

void AAuraPlayerController::AutoRun()
{
	if(!bAutoRunning) return;
	if(APawn* ControlledPawn = GetPawn())
	{
		const FVector LocationOnSpline = Spline->FindLocationClosestToWorldLocation(ControlledPawn->GetActorLocation(),ESplineCoordinateSpace::World);
		const FVector Direction = Spline->FindDirectionClosestToWorldLocation(LocationOnSpline,ESplineCoordinateSpace::World);
		ControlledPawn->AddMovementInput(Direction);

		const float DistanceToDestination = (LocationOnSpline-CachedDestination).Length();
		if(DistanceToDestination <=AutoRunAcceptRadius)
		{
			bAutoRunning = false;
		}
	}
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	//UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);//castcheck会自动检查值的有效性，无效会崩溃，将inputcomponent投射到增强输入组件
	//EnhancedInputComponent->BindAction(MoveAction,ETriggerEvent::Triggered,this,&AAuraPlayerController::Move);//传入moveaction,触发事件，目标（控制器本身），绑定的函数
	//这里将输入组件改成了自己创建的AuraInputComponent,其中创建了一个新的绑定函数,功能是通过Tag检索应该绑定的回调函数

	UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);
	AuraInputComponent->BindAction(MoveAction,ETriggerEvent::Triggered,this,&AAuraPlayerController::Move);
	AuraInputComponent->BindAbilityAction(InputConfig,this,&ThisClass::AbilityInputTagPressed,&ThisClass::AbilityInputTagReleased,&ThisClass::AbilityInputTagHeld);
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


