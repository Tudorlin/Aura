// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService_FindNearestPlayer.h"
#include "AIController.h"
#include "BehaviorTree/BTFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

void UBTService_FindNearestPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* OwningPawn = AIOwner->GetPawn();

	const FName TargetTag = OwningPawn->ActorHasTag(FName("Player")) ? FName("Enemy") : FName("Player");

	TArray<AActor*>ActorWithTags;
	UGameplayStatics::GetAllActorsWithTag(OwningPawn,TargetTag,ActorWithTags);

	float ClosestDistance = TNumericLimits<float>::Max();		//最大浮点数
	AActor* ClosestActor = nullptr;
	for(AActor* Actor : ActorWithTags)
	{
		if(IsValid(Actor)&&IsValid(OwningPawn))
		{
			const float Distance = OwningPawn->GetDistanceTo(Actor);
			if(Distance<ClosestDistance)
			{
				ClosestDistance = Distance;		//检测到玩家的距离小于设定好的最小距离时设置最近距离和最近actor
				ClosestActor = Actor;
			}
		}
	}
	//GEngine->AddOnScreenDebugMessage(-1, .5f, FColor::Orange, *ClosestActor->GetName());
	//设置键值s
	UBTFunctionLibrary::SetBlackboardValueAsObject(this,TargetToFollowSelector,ClosestActor);
	UBTFunctionLibrary::SetBlackboardValueAsFloat(this,DistanceToTargetSelector,ClosestDistance);
}
