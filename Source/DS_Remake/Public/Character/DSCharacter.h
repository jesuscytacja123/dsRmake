// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "Components/HealthComponent.h"
#include "GameFramework/Character.h"
#include "Interfaces/CombatInterface.h"
#include "DSCharacter.generated.h"


class USphereComponent;
class UHealthComponent;
class UBoxComponent;
class UInputMappingContext;
class UInputAction;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class DS_REMAKE_API ADSCharacter : public ACharacter,public ICombatInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADSCharacter();
	
	UFUNCTION(BlueprintCallable)
	void EnableWeaponCollision();

	UFUNCTION(BlueprintCallable)
	void DisableWeaponCollision();

	UFUNCTION(BlueprintCallable)
	void EnableRollCollision();

	UFUNCTION(BlueprintCallable)
	void DisableRollCollision();

	/*Combat Interface Functions*/

	virtual bool GetIsDead() override;
	virtual void GetHit() override;
	
	/*-----------*/

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
protected:

	/*Health Component*/

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UHealthComponent> HealthComponent;

	UPROPERTY(EditDefaultsOnly, Category="Gameplay|Damage")
	float Damage = 25.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,Category="Gameplay|Costs")
	float AttackCost = 25.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Gameplay|Costs")
	float RollCost = 30.f;
	/*------------------*/

	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputMappingContext* DSContext;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MovementAction;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LookAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* RollAction;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* SprintAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* EquipAction;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* AttackAction;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LockAction;
	
	/* Montages */
	UPROPERTY(EditDefaultsOnly, Category="Montages")
	UAnimMontage* EquipMontage;

	UPROPERTY(EditDefaultsOnly, Category="Montages|Combat")
	UAnimMontage* AttackMontage;
	
	UPROPERTY(EditDefaultsOnly, Category="Montages|Combat")
	UAnimMontage* RollMontage;

	UPROPERTY(EditDefaultsOnly, Category="Montage|HitReact")
	UAnimMontage* HitReactMontage;

	
	//related to montage
	FTimerHandle DelayAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LeftRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ForwardBackward;

	UPROPERTY(EditDefaultsOnly, Category="Montage|HitReact")
	TArray<FName> HitReactArray;

	UPROPERTY(EditDefaultsOnly, Category="Montages|Combat")
	TArray<FName> AttacksArray;
	/* End montages */

	void EquipOrUnEquip();

	FName LastAttack;
	
	//Mechanics
	UPROPERTY(EditDefaultsOnly, Category="Combat|Mechanics")
	float AttackSpeed = 0.5f;
	
	FTimerHandle AttackDelay;

	void AttackReset();

	bool bCanAttack = true;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UBoxComponent> BoxCollision;
	
	/* Overlaps */
	
	UPROPERTY(EditDefaultsOnly)
	USceneComponent* BoxTraceStart;

	UPROPERTY(EditDefaultsOnly)
	USceneComponent* BoxTraceEnd;

	/*end*/
	
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Sprint();
	void EndSprint();
	void Equip();
	
	UFUNCTION(BlueprintCallable)
	void Attack();
	
	void LockTarget();

	UPROPERTY(EditDefaultsOnly, Category="Combat")
	USkeletalMeshComponent* WeaponMesh;

private:
	TArray<AActor*> IgnoreActors;

	bool bDead = false;
	
	float DistanceToLock = 2500.f;
	
	TObjectPtr<AActor> Target;
	
	bool bTargetLocked = false;
	
	bool bIsSprinting = false;

	bool bEquippedWeapon = false;
	
	UPROPERTY(VisibleAnywhere, Category="Camera")
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, Category="Camera")
	UCameraComponent* FollowCamera;

	void StartTrace();

	FTimerHandle TraceTimerHandle;
	
	void AttackTrace();
	
	void Die();

	void LookAtSmooth();

	UPROPERTY(EditDefaultsOnly)
	UParticleSystem* Particles;
	
public:
	
	UPROPERTY(BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	bool bWantsToAttack = false;
	
	UPROPERTY(BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	bool bIsRolling = false;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FORCEINLINE bool GetIsSprinting() const { return bIsSprinting; }
	FORCEINLINE bool GetWeaponEquipped() const { return bEquippedWeapon; }
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetIsTargetLocked() const { return bTargetLocked; }
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetbCanAttack() const { return bCanAttack; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE UHealthComponent* GetHealthComponent() const { return HealthComponent; }

	FORCEINLINE UParticleSystem* GetHitParticles() const { return Particles ;}
};
