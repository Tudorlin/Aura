// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraInputConfig.h"
#include "EnhancedInputComponent.h"
#include "AuraInputComponent.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()
public:
	template<class UserClass,typename PressFncType,typename ReleasedFncType,typename HeldFncType>
	void BindAbilityAction(const UAuraInputConfig* InputConfig,UserClass* Object,PressFncType PressFnc,ReleasedFncType ReleasedFnc,HeldFncType HeldFnc);

	
};

template <class UserClass, typename PressFncType, typename ReleasedFncType, typename HeldFncType> //定义的函数会在玩家控制器类中的输入函数中调用，用于绑定不同按键模式下对应Tag的函数以及输入操作
void UAuraInputComponent::BindAbilityAction(const UAuraInputConfig* InputConfig, UserClass* Object,
	PressFncType PressFnc, ReleasedFncType ReleasedFnc, HeldFncType HeldFnc)
{
	check(InputConfig);
	for(const auto& Action : InputConfig->AbilityInputActions)
	{
		if(Action.InputAction&&Action.InputTag.IsValid())
		{
			if(PressFnc)
			{
				BindAction(Action.InputAction,ETriggerEvent::Started,Object,PressFnc,Action.InputTag);    //此绑定函数在之前绑定移动的时候其实已经见过了，是增强输入的成员函数，
			}
			if(ReleasedFnc)
			{
				BindAction(Action.InputAction,ETriggerEvent::Completed,Object,ReleasedFnc,Action.InputTag);
			}
			if(HeldFnc)
			{
				BindAction(Action.InputAction,ETriggerEvent::Triggered,Object,HeldFnc,Action.InputTag);
			}
		}
	}
}
