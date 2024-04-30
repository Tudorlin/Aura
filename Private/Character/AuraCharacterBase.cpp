
#include "Character/AuraCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Aura/Aura.h"
#include "Components/CapsuleComponent.h"


AAuraCharacterBase::AAuraCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;


	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera,ECR_Ignore);
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera,ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Projectile,ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);   //生成重叠事件

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(),FName("WeaponHandSocket"));
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

UAbilitySystemComponent* AAuraCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAnimMontage* AAuraCharacterBase::GetHitMontage_Implementation()
{
	return HitReactMontage;
}

void AAuraCharacterBase::Die()
{
	Weapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld,true));
	Dissolve();
	MulticastHandleDeath();
}

void AAuraCharacterBase::Dissolve()
{
	if(IsValid(DissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* DynamicInstance = UMaterialInstanceDynamic::Create(DissolveMaterialInstance,this);	//创建动态材质实例
		GetMesh()->SetMaterial(0,DynamicInstance);				//将材质赋予模型
		StartDissolveTimeline(DynamicInstance);				//启用溶解效果
	}
	if(IsValid(WeaponDissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* WeaponDynamicInstance = UMaterialInstanceDynamic::Create(WeaponDissolveMaterialInstance,this);
		Weapon->SetMaterial(0,WeaponDynamicInstance); 
		StartWeaponDissolveTimeline(WeaponDynamicInstance);
	}
}


void AAuraCharacterBase::MulticastHandleDeath_Implementation()
{
	Weapon->SetSimulatePhysics(true);			//对于武器，启用物理模拟
	Weapon->SetEnableGravity(true);
	Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	GetMesh()->SetSimulatePhysics(true);			//对于网格体模型，启用模拟物理的同时开启地面与网格的碰撞防止掉出场景并取消胶囊体的碰撞
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic,ECR_Block);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AAuraCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

FVector AAuraCharacterBase::GetCombatSocketLocation()
{
	check(Weapon);
	return Weapon->GetSocketLocation(WeaponTipSocketName);
}

void AAuraCharacterBase::InitAbilityActorInfo()
{
}

void AAuraCharacterBase::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float level) const
{
	check(IsValid(GetAbilitySystemComponent()));
	check(GameplayEffectClass);
	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	ContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(GameplayEffectClass,level,ContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(),GetAbilitySystemComponent());
}

void AAuraCharacterBase::InitializeDefaultAttributes() const
{
	ApplyEffectToSelf(DefaultPrimaryAttributes,1.f);
	ApplyEffectToSelf(DefaultSecondaryAttributes,1.f);
	ApplyEffectToSelf(DefaultVitalAttributes,1.f);
}

void AAuraCharacterBase::AddCharacterAbility()
{
	UAuraAbilitySystemComponent* AuraASC = CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent);
	if(!HasAuthority()) return;  //在服务端上添加能力
	AuraASC->AddCharacterAbilities(StartupAbilities);
}

// void AAuraCharacterBase::InitializePrimaryAttributes() const
// {
// 	check(IsValid(GetAbilitySystemComponent()));
// 	check(DefaultPrimaryAttributes);
// 	const FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
// 	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(DefaultPrimaryAttributes,1.f,ContextHandle);
// 	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(),GetAbilitySystemComponent());
// }


