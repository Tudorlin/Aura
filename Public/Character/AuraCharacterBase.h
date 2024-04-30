
#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "Interfaction/CombatInterface.h"
#include "AuraCharacterBase.generated.h"

class UGameplayAbility;
class UGameplayEffect;
class UAttributeSet;
class UAbilitySystemComponent;

UCLASS()
class AURA_API AAuraCharacterBase : public ACharacter,public IAbilitySystemInterface,public ICombatInterface
{
	GENERATED_BODY()

public:
	AAuraCharacterBase();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UAttributeSet* GetAttributeSet() const { return AttributeSet; }

	virtual UAnimMontage* GetHitMontage_Implementation() override;

	virtual void Die() override;

	UFUNCTION(NetMulticast,Reliable)
	virtual void MulticastHandleDeath();
protected:
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere,Category="Combat")
	TObjectPtr<USkeletalMeshComponent>Weapon;   //武器，不知从哪个版本开始ue用TObject和FObject代替了某些裸指针，作用好像是方便追踪，后面再细看
	//基类用作玩家角色和AI敌人的父类，所以在基类中不需要tick和SetupPlayerInputComponent函数,输入处理将在控制器中实现

	UPROPERTY(EditAnywhere,Category="Combat")
	FName WeaponTipSocketName;   //法杖发射物插槽的名字

	virtual FVector GetCombatSocketLocation() override;
	

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	virtual void InitAbilityActorInfo();

	UPROPERTY(BlueprintReadOnly,EditAnywhere,Category="Attributes")
	TSubclassOf<UGameplayEffect>DefaultPrimaryAttributes;

	UPROPERTY(BlueprintReadOnly,EditAnywhere,Category="Attributes")
	TSubclassOf<UGameplayEffect>DefaultSecondaryAttributes;

	UPROPERTY(BlueprintReadOnly,EditAnywhere,Category="Attributes")
	TSubclassOf<UGameplayEffect>DefaultVitalAttributes;

	//void InitializePrimaryAttributes() const;     由于两个初始化函数差不多，所以定义为传入一个GameplayEffect类作参数的函数
	//void InitializeSecondaryAttributes() const;

	void ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass,float level) const;
	virtual void InitializeDefaultAttributes() const;

	void AddCharacterAbility();

	//死亡后的溶解效果
	void Dissolve();
	
	UFUNCTION(BlueprintImplementableEvent)
	void StartDissolveTimeline(UMaterialInstanceDynamic* DynamicMaterialInstance);

	UFUNCTION(BlueprintImplementableEvent)
	void StartWeaponDissolveTimeline(UMaterialInstanceDynamic* DynamicWeaponMaterialInstanceDynamic);

	UPROPERTY(EditAnywhere,Category="Dissolve")
	TObjectPtr<UMaterialInstance> DissolveMaterialInstance;

	UPROPERTY(EditAnywhere,Category=Dissolve)
	TObjectPtr<UMaterialInstance> WeaponDissolveMaterialInstance;

private:
	UPROPERTY(EditAnywhere,Category="Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

	UPROPERTY(EditAnywhere,Category="Combact")
	TObjectPtr<UAnimMontage> HitReactMontage;
};
