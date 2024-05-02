#pragma once
#include "GameplayEffectTypes.h"
#include "AuraAbilityType.generated.h"


//GAS自带的上下文不能获取某些自定义的效果，例如是否暴击、是否被格挡等，以其为父类派生出自定自定义的上下文，以满足自定义的需求
USTRUCT(BlueprintType)
struct  FAuraGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()

public:

	bool IsBlocked() const { return bIsBlocked; }
	bool IsCriticalHit() const { return bIsCriticalHit; }

	void SetIsBlocked(bool Blocked) {bIsBlocked = Blocked;}
	void SetIsCriticalHit(bool CriticalHit) {bIsCriticalHit = CriticalHit;}

	virtual UScriptStruct* GetScriptStruct() const
	{
		return StaticStruct();
	}

	//复制函数，后续的计算使用复制的版本
	virtual FAuraGameplayEffectContext* Duplicate() const
	{
		FAuraGameplayEffectContext* NewEffectContext = new FAuraGameplayEffectContext();
		*NewEffectContext = *this;
		if(GetHitResult())
		{
			NewEffectContext->AddHitResult(*GetHitResult(),true);	//HitResult使用深拷贝
		}
		return NewEffectContext;
	}

	virtual bool NetSerialize(FArchive& Ar,class UPackageMap* Map,bool& bOutSuccess);		//此函数在FGameplayEffectContext中也有，这里依据原版实现
protected:

	UPROPERTY()
	bool bIsBlocked = false;

	UPROPERTY()
	bool bIsCriticalHit = false;
};

//这个模板在FGameplayEffectContext中也有实现，同样也是仿造
template<>
struct TStructOpsTypeTraits<FAuraGameplayEffectContext> : public TStructOpsTypeTraitsBase2<FAuraGameplayEffectContext>
	/*
	*TStructOpsTypeTraitsBase2定义了结构体的一些特性，用于描述结构体的自定义行为，原结构体中定义的行为多为false,如果想自定义某些行为，可以继承这个结构体进行设置
	   WithZeroConstructor：如果为 true，表示该结构体可以通过将其内存填充为零来构造为有效对象。
	   WithNoInitConstructor：如果为 true，表示该结构体有一个接受 EForceInit 参数的构造函数，该参数将强制构造函数执行初始化，而默认构造函数执行“未初始化”。
	   WithNoDestructor：如果为 true，表示在销毁该结构体时不会调用其析构函数。
	   WithCopy：如果为 true，表示该结构体可以通过其复制赋值运算符进行复制。
	   WithIdenticalViaEquality：如果为 true，表示该结构体可以通过其 operator== 进行比较。这应与 WithIdentical 互斥。
	   WithIdentical：如果为 true，表示该结构体可以通过一个 Identical(const T* Other, uint32 PortFlags) 函数进行比较。这应与 WithIdenticalViaEquality 互斥。
	   WithExportTextItem：如果为 true，表示该结构体有一个 ExportTextItem 函数，用于将其状态序列化为字符串。
	   WithImportTextItem：如果为 true，表示该结构体有一个 ImportTextItem 函数，用于将字符串反序列化为该类的对象。
	   WithAddStructReferencedObjects：如果为 true，表示该结构体有一个 AddStructReferencedObjects 函数，允许它向垃圾收集器添加引用。
	   WithSerializer：如果为 true，表示该结构体有一个 Serialize 函数，用于将其状态序列化到 FArchive。
	   WithStructuredSerializer：如果为 true，表示该结构体有一个 Serialize 函数，用于将其状态序列化到 FStructuredArchive。
	   WithPostSerialize：如果为 true，表示该结构体有一个 PostSerialize 函数，该函数在序列化后被调用。
	   WithNetSerializer：如果为 true，表示该结构体有一个 NetSerialize 函数，用于将其状态序列化到用于网络复制的 FArchive。
	*/
{
	enum
	{
		WithNetSerializer = true,		//表示该结构体有一个 Serialize 函数，用于将其状态序列化到 FArchive
		WithCopy = true					//表示该结构体可以通过其复制赋值运算符进行复制。
	};
};

