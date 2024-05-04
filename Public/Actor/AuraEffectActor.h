// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameFramework/Actor.h"
#include "AuraEffectActor.generated.h"

class UGameplayEffect;
//class USphereComponent;

UENUM(BlueprintType)
enum class EEffectApplicationPolicy   //用于判断效果应用方式
{
	ApplyOnOverlap,
	ApplyOnEndOverlap,
	DoNotApply
};

UENUM(BlueprintType)
enum class EEffectRemovalPolicy   //用于判断是否移除无限持续效果
{
	RemoveOnEndOverlap,
	DoNotRemove
};

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

	UFUNCTION(BlueprintCallable)
	void OnOverlap(AActor* TargetActor);

	UFUNCTION(BlueprintCallable)
	void OnEndOverlay(AActor*TargetActor);

	bool bDestroyOnEffectApplied = false;	//判断GE被应用后是否销毁

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Effects")
	bool bApplyEffectToEnemies = false;		//判定是否对敌人有效	

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Effects")
	bool bDestroyOnEffectRemoval = false;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Effects")
	TSubclassOf<UGameplayEffect>InstantGameplayEffectClass;  //瞬间效果:立即生效且只生效一次
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Effects")
	EEffectApplicationPolicy InstantEffectApplicationPolicy = EEffectApplicationPolicy::DoNotApply;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Effects")
	TSubclassOf<UGameplayEffect>DurationGameplayEffectClass;//持续效果:持续一段设定好的时间，时间过了之后效果消失
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Effects")
	EEffectApplicationPolicy DurationEffectApplicationPolicy = EEffectApplicationPolicy::DoNotApply;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Effects")
	TSubclassOf<UGameplayEffect>InfiniteGameplayEffectClass;   //无限持续时间效果
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Effects")
	EEffectApplicationPolicy InfinityEffectApplicationPolicy = EEffectApplicationPolicy::DoNotApply;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Effects")
	EEffectRemovalPolicy InfinityEffectRemovalPolicy = EEffectRemovalPolicy::RemoveOnEndOverlap;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Effects")
	float EffectLevel = 1.f;

	TMap<FActiveGameplayEffectHandle,UAbilitySystemComponent*>ActiveEffectHandles;

	
	/**
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> Sphere;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> Mesh;***/
	

};
