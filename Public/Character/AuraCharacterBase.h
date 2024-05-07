
#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "GameFramework/Character.h"
#include "Interfaction/CombatInterface.h"
#include "AuraCharacterBase.generated.h"

class UNiagaraSystem;
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

	//CombatInterface
	virtual void Die() override;
	virtual FVector GetCombatSocketLocation_Implementation(const FGameplayTag& CombatSocketTag) override;
	virtual UAnimMontage* GetHitMontage_Implementation() override;
	virtual bool IsDead_Implementation() const override;
	virtual AActor* GetAvatar_Implementation() override;
	virtual TArray<FTaggedMontage> GetAttackMontages_Implementation() override;
	virtual UNiagaraSystem* GetBloodEffect_Implementation() override;
	virtual FTaggedMontage GetTaggedMontageByTag_Implementation(const FGameplayTag& MontageTag) override;
	virtual int32 GetMinionCount_Implementation() override;
	virtual void IncrementMinionCount_Implementation(int32 Amount) override;
	virtual ECharacterClass GetCharacterClass_Implementation() override;
	//CombatInterface
	

	UFUNCTION(NetMulticast,Reliable)
	virtual void MulticastHandleDeath();

	UPROPERTY(EditAnywhere,Category="Combat")		//角色蓝图类中添加Tag和蒙太奇,通过CombatInterface中的Get函数获取并使用
	TArray<FTaggedMontage> AttackMontage;
protected:
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Combat")
	TObjectPtr<USkeletalMeshComponent>Weapon;   //武器，不知从哪个版本开始ue用TObject和FObject代替了某些裸指针，作用好像是方便追踪，后面再细看
	//基类用作玩家角色和AI敌人的父类，所以在基类中不需要tick和SetupPlayerInputComponent函数,输入处理将在控制器中实现

	UPROPERTY(EditAnywhere,Category="Combat")
	FName WeaponTipSocketName;   //武器插槽名字

	UPROPERTY(EditAnywhere,Category="Combat")
	FName LeftHandSocketName;		//某个怪物使用爪子进行攻击，两只手上对应的插槽名字

	UPROPERTY(EditAnywhere,Category="Combat")
	FName RightHandSocketName;
	
	UPROPERTY(EditAnywhere,Category="Combat")
	FName TailSocketName;
	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	bool bDead = false;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	virtual void InitAbilityActorInfo();

	UPROPERTY(BlueprintReadOnly,EditAnywhere,Category="Attributes")
	TSubclassOf<UGameplayEffect>DefaultPrimaryAttributes;

	UPROPERTY(BlueprintReadOnly,EditAnywhere,Category="Attributes")
	TSubclassOf<UGameplayEffect>DefaultSecondaryAttributes;

	UPROPERTY(BlueprintReadOnly,EditAnywhere,Category="Attributes")
	TSubclassOf<UGameplayEffect>DefaultVitalAttributes;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Character Default Class")  //敌人类型
	ECharacterClass CharacterClass = ECharacterClass::Warrior;		//角色类型移至Base类中，通过继承接口的函数获取

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

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	UNiagaraSystem* BloodEffect;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Combat")
	USoundBase* DeathSound;

	int32 MinionCount = 0;//召唤个数

private:
	UPROPERTY(EditAnywhere,Category="Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

	UPROPERTY(EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupPassiveAbilities;

	UPROPERTY(EditAnywhere,Category="Combact")
	TObjectPtr<UAnimMontage> HitReactMontage;
};
