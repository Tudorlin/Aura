// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilityTasks/TargetDataUnderMouse.h"

#include "AbilitySystemComponent.h"

UTargetDataUnderMouse* UTargetDataUnderMouse::CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility)
{
	UTargetDataUnderMouse* MyObj = NewAbilityTask<UTargetDataUnderMouse>(OwningAbility);  //创建一个UTargetDataUnderMouse，其拥有者为传入的UGameplayAbility
	return MyObj;
}

void UTargetDataUnderMouse::Activate()   //Task被激活时调用
{
	const bool bIsLocallyControlled = Ability->GetCurrentActorInfo()->IsLocallyControlled();
	if(bIsLocallyControlled)     //如果是客户端，则将数据发送给服务器
	{
		SendMouseData();
	}
	else						//如果是服务器，则收集数据
	{
		const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
		const FPredictionKey PredictionKey = GetActivationPredictionKey();
		AbilitySystemComponent.Get()->AbilityTargetDataSetDelegate(SpecHandle,PredictionKey).AddUObject(
			this,&UTargetDataUnderMouse::OnTargetDataReplicatedCallback);  //TargetData被复制时触发回调
		const bool bCalledDelegate = AbilitySystemComponent.Get()->CallReplicatedTargetDataDelegatesIfSet(SpecHandle,PredictionKey);  //委托被设置为真，否则为假
		if(!bCalledDelegate)
		{
			SetWaitingOnRemotePlayerData();    //等待远程角色的数据
		}
	}
}

void UTargetDataUnderMouse::SendMouseData()
{
	APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();   //这里的Ability存在于UGameplayTasks中
	FHitResult CursorHit;
	PC->GetHitResultUnderCursor(ECC_Visibility,false,CursorHit);
	FGameplayAbilityTargetDataHandle DataHandle;
	FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
	Data->HitResult = CursorHit;
	DataHandle.Add(Data);

	AbilitySystemComponent->ServerSetReplicatedTargetData(      //将数据发送到服务器
		GetAbilitySpecHandle(),
		GetActivationPredictionKey(),
		DataHandle,
		FGameplayTag(),
		AbilitySystemComponent->ScopedPredictionKey);

	
	ValidData.Broadcast(DataHandle);     //广播鼠标位置
}

void UTargetDataUnderMouse::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle,
	FGameplayTag ActivationTag)
{
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(),GetActivationPredictionKey());
	//void ConsumeClientReplicatedTargetData(FGameplayAbilitySpecHandle AbilityHandle,FPredictionKey AbilityOriginalPredictionKey)的作用是消耗客户端复制的TargetData,用于服务器和客户端之间的同步
	if(ShouldBroadcastAbilityTaskDelegates())   //能力处于激活状态则返回true,否则返回false，通常在广播委托回到能力Map之前调用，确保能力仍在激活状态
	{
		ValidData.Broadcast(DataHandle);
	}
}
