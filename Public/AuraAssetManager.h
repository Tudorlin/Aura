// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "AuraAssetManager.generated.h"

/**
 * UAuraAssetManager 是一个 Unreal Engine 中的类，用于管理游戏中的资源。
 */
UCLASS()
class AURA_API UAuraAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	static UAuraAssetManager&Get();


protected:
	virtual void StartInitialLoading() override;  //此函数用于触发游戏启动时一些必要资源加载的操作，包括地图，材质，纹理之类的资源，在此处对Tags进行初始化，以保证游戏开始时tag能初始化完毕。\

};
