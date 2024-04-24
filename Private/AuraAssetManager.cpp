// Fill out your copyright notice in the Description page of Project Settings.


#include "AuraAssetManager.h"

#include "AbilitySystemGlobals.h"
#include "AuraGameplayTags.h"


UAuraAssetManager& UAuraAssetManager::Get()
{
	check(GEngine);
	UAuraAssetManager* AuraAssetManager = Cast<UAuraAssetManager>(GEngine->AssetManager);
	return *AuraAssetManager;  //注意返回的是引用
}

void UAuraAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
	FAuraGameplayTags::InitializeNativeGameplayTags();

	UAbilitySystemGlobals::Get().InitGlobalData();
	/*这个函数应该在项目设置的一部分中被调用一次，以加载全局数据表和标签。从UE4.24开始，如果要使用TargetData，
	 *就必须调用UAbilitySystemGlobals::Get().InitGlobalData()，否则会遇到ScriptStructCache相关的错误，且客户端将从服务器断开连接
	在项目中，这个方法只需要调用一次。例如，可以在UAssetManager::StartInitialLoading()或者UEngine::Init()中调用它。*/
}
