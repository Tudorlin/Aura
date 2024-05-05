// Fill out your copyright notice in the Description page of Project Settings.


#include "..\..\..\Public\AbilitySystem\Ability\AuraProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "Actor/AuraProjectile.h"
#include "Interfaction/CombatInterface.h"
#include "Kismet/KismetSystemLibrary.h"

void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	
}

void UAuraProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation,const FGameplayTag& SocketTag)
{
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();    //检查是否为服务器
	if(!bIsServer) return;
	// ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo());  //从当前能力获取AvatarActor.将其转成战斗接口，调用其中的获取插槽的位置细信息函数
	// if(CombatInterface)
	// {
	const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(),SocketTag);	//使用BlueprintNativeEvent或BlueprintImplementableEvent修饰的函数要用这种调用方式调用执行版本，
	FRotator Rotation = (ProjectileTargetLocation-SocketLocation).Rotation();

	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SocketLocation);
	SpawnTransform.SetRotation(Rotation.Quaternion());   //FQuat是另一种表示旋转的方式，即使用四元组来表示旋转
	AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(  //和一普通的Spawn函数有区别，这个Spawn是延迟生成，它不会立即执行蓝图的构造脚本，而是允许程序员
//在生成过程完成前强制设置世界变换（包括位置、旋转和缩放）;参数设置完了之后需要调用FinishSpawning完成生成过程
		ProjectileClass,
		SpawnTransform,
		GetOwningActorFromActorInfo(),
		Cast<APawn>(GetOwningActorFromActorInfo()),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());  //获取能力拥有者的能力组件
	FGameplayEffectContextHandle ContextHandle = SourceASC->MakeEffectContext();		//创建并设置上下文句柄
	ContextHandle.SetAbility(this);
	ContextHandle.AddSourceObject(Projectile);
	TArray<TWeakObjectPtr<AActor>>Actors;
	Actors.Add(Projectile);
	ContextHandle.AddActors(Actors);
	FHitResult HitResult;
	HitResult.Location = ProjectileTargetLocation;
	ContextHandle.AddHitResult(HitResult);
	const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectsClass,GetAbilityLevel(),ContextHandle);  //通过获取到的能力组件创建一个SpecHandle
		
		

	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();

	for(auto Pair : DamageType)
	{
		const float ScaleDamage = Pair.Value.GetValueAtLevel(GetAbilityLevel());
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle,Pair.Key,ScaleDamage);
	}
	//const float ScaledDamage = Damage.GetValueAtLevel(GetAbilityLevel());
	//UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle,GameplayTags.Damage,ScaledDamage);	//将值赋予Tag对应的SetByCaller用于计算
	Projectile->DamageEffectSpecHandle = SpecHandle;   //将创建的SpecHandle保存在GA中的变量中,将会在Projectile中用于计算伤害

	Projectile->FinishSpawning(SpawnTransform);
	//}
}
