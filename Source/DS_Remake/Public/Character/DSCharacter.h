// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "DSCharacter.generated.h"


class UInputMappingContext;
class UInputAction;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class DS_REMAKE_API ADSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADSCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputMappingContext* DSContext;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MovementAction;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LookAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* SprintAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* EquipAction;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* AttackAction;

	
	/* Montages */
	UPROPERTY(EditDefaultsOnly, Category="Montages")
	UAnimMontage* EquipMontage;

	UPROPERTY(EditDefaultsOnly, Category="Montages|Combat")
	UAnimMontage* AttackMontage;
	//related to montage
	FTimerHandle DelayAnim;


	
	
	
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

	
	
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Sprint();
	void EndSprint();
	void Equip();
	void Attack();

	UPROPERTY(EditDefaultsOnly, Category="Combat")
	USkeletalMeshComponent* WeaponMesh;

private:

	bool bIsSprinting = false;

	bool bEquippedWeapon = false;
	
	UPROPERTY(VisibleAnywhere, Category="Camera")
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, Category="Camera")
	UCameraComponent* FollowCamera;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FORCEINLINE bool GetIsSprinting() const { return bIsSprinting; }
	FORCEINLINE bool GetWeaponEquipped() const { return bEquippedWeapon; }

};
