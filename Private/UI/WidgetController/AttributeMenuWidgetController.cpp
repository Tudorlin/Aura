// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"


void UAttributeMenuWidgetController::BroadcastInitialValue()
{
	const UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributesSet);   //AuraWidgetController中的属性集,存有主要属性和次要属性
	check(AttributeInfo);    //派生的蓝图中传入的数据资产，包含了FAuraAttributeInfo结构体构成的数组，前者包含了Tag及其名字和描述以及Tag对应属性的值

	for(auto TagToAttribute : AS->TagToAttributes)     //储存在属性集中的Map中包含Tag以及Tag对应属性的属性访问器
	{
		BroadcastAttributeInfo(TagToAttribute.Key,TagToAttribute.Value());
	}
}

void UAttributeMenuWidgetController::BindCallbacksDependencies()
{
	const UAuraAttributeSet* AS = Cast<UAuraAttributeSet>(AttributesSet);
	check(AttributeInfo)

	for(auto Pair : AS->TagToAttributes)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
			[this,Pair](const FOnAttributeChangeData& Data)
			{
					BroadcastAttributeInfo(Pair.Key,Pair.Value());
			});
	}
}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag,
	const FGameplayAttribute& Attribute) const
{
	FAuraAttributeInfo Info=AttributeInfo->FindAttributeInfoForTag(AttributeTag);  //根据属性集中Map的Key在数据资产数组中查找对应的属性信息，此处查找得到的是一个FAuraAttributeInfo结构体
	Info.AttributeValue = Attribute.GetNumericValue(AttributesSet);  //从属性集中获取值设置到上面获取到的结构体,GetNumericValue函数用于获取指定属性值，无论是数值还是属性数据
	//由于传入的实参是Pair.Value()，按函数的性质来说只要加了()就代表着调用了，所以绑定的属性访问器会被调用，获取到的是属性数据
	AttributeInfoDelegate.Broadcast(Info);   //将此结构体的信息广播给菜单中的子控件，子控件按照自己的Tag进行检索并设置各个信息。
}
