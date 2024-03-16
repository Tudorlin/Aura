// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/AuraEffectActor.h"

//#include "AbilitySystemInterface.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
//#include "Components/SphereComponent.h"
//#include "Misc/RuntimeErrors.h"

AAuraEffectActor::AAuraEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;
	/*
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(Mesh);
	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	Sphere->SetupAttachment(GetRootComponent());
	*/
	SetRootComponent(CreateDefaultSubobject<USceneComponent>("RootComponent"));  //源码中给一个场景组件即可
}

/*void AAuraEffectActor::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherCamp, int32 OtherBodyIndex, bool bFromSweep,const FHitResult& SweepResult)
{
	//现阶段用const_cast破坏封装性以访问私有属性集
	if(IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(OtherActor))  //将碰撞到actor转化为ASC接口类
	{
		const UAuraAttributeSet* AuraAttributeSet = Cast<UAuraAttributeSet>(ASCInterface
			->GetAbilitySystemComponent()->GetAttributeSet(UAuraAttributeSet::StaticClass()));
		/** 这里用一个AuraAttributesSet类型的指针保存碰撞对象的属性集，具体是调用接口中重写的GetAbilitySystemComponent()函数(在CharacterBase类中),
		 *然后通过能力组件获取属性集。StaticClass()是一个虚函数，用于获取当前类的运行类时的类型信息,会返回UAuraAttributeSet的UClass对象 
		
		UAuraAttributeSet* AttributeSet = const_cast<UAuraAttributeSet*>(AuraAttributeSet);
		AttributeSet->SetHealth(AttributeSet->GetHealth()+25.f);
		AttributeSet->SetMana(AttributeSet->GetMana()+25.f);
		Destroy();
	}
} */

void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();
	/*
	Sphere->OnComponentBeginOverlap.AddDynamic(this,&AAuraEffectActor::OnOverlap);  //绑定函数
	Sphere->OnComponentEndOverlap.AddDynamic(this,&AAuraEffectActor::EndOverlap);
	*/
}

void AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
	//获取target的能力组件
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if(TargetASC==nullptr) return;
	check(GameplayEffectClass);
	FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();//创建上下文句柄
	/**
	 *FGameplayEffectContextHandle : 包装了FGameplayEffectContext或其子类，便于他能多态的使用且能够正确的复制；其提供了一系列函数用于添加或者获取效果的
	 *相关信息，例如施法者，效果来源，效果目标等，命中结果等;这些信息应该是储存在共享指针Data指向的FGameplayEffectContext中。
	 ***/
	EffectContextHandle.AddSourceObject(this);    //当前对象作为效果来源添加进上下文
	const FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass,1.f,EffectContextHandle);
	/**FGameplayEffectSpecHandle允许蓝图生成一个FGameplayEffectSpec并通过句柄引用，即可以在蓝图或代码中引用和复制有游戏效果，不需要担心原始对象的内存管理
	 *函数:Clear():清除句柄中的数据;	IsValid():检查句柄是否有效		NetSerialize():用于网络复制序列化操作。此外还重载了操作符用于判断两个句柄是否相等**/
	TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());//将定义在GameplayEffectClass中的效果应用在Target身上。
	/**FGameplayEffectSpecHandle中的Data是一个指向FGameplayEffectSpec的共享指针，FGameplayEffectSpec包含了定义游戏效果的所有信息,
	 *包括不限于持续时间，强度，对属性的影响等
	 *此外ApplyGameplayEffcetSpecToSelf与ApplyGameplayEffectToSelf的区别在于前者提供了更多自定义的能力，允许开发者在应用效果前调整规格，而后者通常不需要额外的配置
	 *用于快速应用一个标准的效果；举个通俗例子:hp药水直接回复50血，则直接用后者；各种装备提供属性增益，这是多种不同的增益叠加，则用前者更好。
	 ***/
}

