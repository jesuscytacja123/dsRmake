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

	/*Combat Interface Functions*/

	virtual bool GetIsDead() override;
	virtual void GetHit() override;
	
	/*-----------*/
	
protected:

	/*Health Component*/

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UHealthComponent> HealthComponent;

	UPROPERTY(EditDefaultsOnly, Category="Gameplay|Damage")
	float Damage = 25.f;
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
	//related to montage
	FTimerHandle DelayAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LeftRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ForwardBackward;
	
	/* End montages */

	void EquipOrUnEquip();

	UPROPERTY(EditDefaultsOnly, Category="Montages|Combat")
	TArray<FName> AttacksArray;

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

	TArray<AActor*> IgnoreActors;
	
	

	

	
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


	bool bDead = false;
	
	float DistanceToLock = 800.f;
	
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
	
};
