// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AuraEffectActor.generated.h"

class UGameplayEffect;
//class USphereComponent;

UCLASS()
class AURA_API AAuraEffectActor : public AActor
{
	GENERATED_BODY()
	 
public:	
	AAuraEffectActor();
	/******            *****为了给设计师更多工作空间，移除固定的静态网格与碰撞体，在保证功能的情况下由设计师自行diy
	UFUNCTION()
	virtual void OnOverlap(UPrimitiveComponent* OverlappedComponent,AActor*OtherActor,UPrimitiveComponent*OtherCamp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult);
	/****几个参数的含义分别是:该actor发生碰撞的组件；与该actor发生碰撞的actor；另一个actor发生碰撞的组件;体积索引:当某个物体被分开建模时，不同的部位可能需要不同的物理性质，所以
	 *为了将各个体积做出区分，就会使用体积索引；如果是单体积的，通常索引为0;布尔为判断是否有扫描操作。另外这个UPrimitiveComponent继承自场景组件，他包含了所有可以进行物理模拟和碰撞检测的组件
	UFUNCTION()
	virtual void EndOverlap(UPrimitiveComponent* OverlappedComponent,AActor*OtherACtor,UPrimitiveComponent*OtherCamp,int32 OtherBodyIndex);
	******/
protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void ApplyEffectToTarget(AActor* TargetActor,TSubclassOf<UGameplayEffect>GameplayEffectClass);

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Effects")
	TSubclassOf<UGameplayEffect>InstantGameplayEffectClass;  //瞬间效果:立即生效且只生效一次
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Effects")
	TSubclassOf<UGameplayEffect>DurationGameplayEffectClass;//持续效果:持续一段设定好的时间，时间过了之后效果消失
/**
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> Sphere;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> Mesh;***/
	

};
