// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbility/AuraSummonAbility.h"

#include "Kismet/KismetSystemLibrary.h"

TArray<FVector> UAuraSummonAbility::GetSpawnLocation()
{
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();		//当前actor的前向向量
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	const float DeltaSpread = SpawnSpread/MinionNum;

	const FVector LeftOfSpread = Forward.RotateAngleAxis(-SpawnSpread/2,FVector::UpVector); //Z轴正向单位向量,绕Z轴旋转-SpawnSpread/2度，后面的操作相当于设置范围以Forward向量为中点，两边各占SpawnSpread/2度的扇形区域
	TArray<FVector>SpawnLocations;
	for(int32 i = 0;i<MinionNum;i++)
	{
		const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread*i,FVector::UpVector);//以LeftOfSpread为起始向量，每隔DeltaSpread度创建一个共MinionNum个向量
		FVector SpawnLocation = Location+Direction*FMath::RandRange(MinSpawnDistance,MaxSpawnDistance);	//生成位置为当前方向，自定义长度的线段上的随机一点

		FHitResult Hit;
		GetWorld()->LineTraceSingleByChannel(Hit,SpawnLocation+FVector(0.f,0.f,400.f),
			SpawnLocation-FVector(0.f,0.f,400.f),ECC_Visibility);		//检测生成点位为中点，上下各400距离的区域
		if(Hit.bBlockingHit)
		{
			SpawnLocations.Add(SpawnLocation);	//防止生成在某些过高或者过低区域或者没有地面。
		}
		/* DrawDebugSphere(GetWorld(), SpawnLocation, 18.f, 12, FColor::Cyan, false, 3.f );
		// UKismetSystemLibrary::DrawDebugArrow(GetAvatarActorFromActorInfo(), Location, Location + Direction * MaxSpawnDistance, 4.f, FLinearColor::Green, 3.f );
		// DrawDebugSphere(GetWorld(), Location + Direction * MinSpawnDistance, 5.f, 12, FColor::Red, false, 3.f );
		// DrawDebugSphere(GetWorld(), Location + Direction * MaxSpawnDistance, 5.f, 12, FColor::Red, false, 3.f );*/
	}
	return SpawnLocations;
}

TSubclassOf<APawn> UAuraSummonAbility::GetRandomMinionClass()
{
	const int32 Selection = FMath::RandRange(0,MinionClass.Num()-1);	//随机选择召唤的怪物
	return MinionClass[Selection];
}
