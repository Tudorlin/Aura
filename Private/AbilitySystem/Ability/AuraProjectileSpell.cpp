// Fill out your copyright notice in the Description page of Project Settings.


#include "..\..\..\Public\AbilitySystem\Ability\AuraProjectileSpell.h"

#include "Actor/AuraProjectile.h"
#include "Interfaction/CombatInterface.h"
#include "Kismet/KismetSystemLibrary.h"

void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	const bool bIsServer = HasAuthority(&ActivationInfo);    //检查是否为服务器
	if(!bIsServer) return;


	ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo());  //从当前能力获取AvatarActor.将其转成战斗接口，调用其中的获取插槽的位置细信息函数
	if(CombatInterface)
	{
		const FVector SocketLocation = CombatInterface->GetCombatSocketLocation();

		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SocketLocation);
		AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(  //和一普通的Spawn函数有区别，这个Spawn是延迟生成，它不会立即执行蓝图的构造脚本，而是允许程序员
//在生成过程完成前强制设置世界变换（包括位置、旋转和缩放）;参数设置完了之后需要调用FinishSpawning完成生成过程
			ProjectileClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetOwningActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		Projectile->FinishSpawning(SpawnTransform);
	}
	
}
