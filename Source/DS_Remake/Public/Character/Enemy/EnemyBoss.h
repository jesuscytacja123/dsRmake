// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/HealthComponent.h"
#include "GameFramework/Character.h"
#include "EnemyBoss.generated.h"

class UPawnSensingComponent;
class UWidgetComponent;
class UHealthComponent;
class AAIController;

UCLASS()
class DS_REMAKE_API AEnemyBoss : public ACharacter
{
	GENERATED_BODY()

public:

	AEnemyBoss();
	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UWidgetComponent> LockDot;

	UFUNCTION()
	void PawnSeen(APawn* Pawn);

protected:
	
	virtual void BeginPlay() override;

	TObjectPtr<AAIController> BossController;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UHealthComponent> HealthComponent;

	UPROPERTY()
	TObjectPtr<AActor> CombatCharacter;

	UPROPERTY(EditDefaultsOnly)
	UPawnSensingComponent* PawnSensingComponent;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USceneComponent> StartCollision;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USceneComponent> EndCollision;

	void AttackReset();
private:

	UPROPERTY(EditDefaultsOnly, Category="Combat")
	UAnimMontage* AttackMontage;

	const FName Attack1 = FName("Swing1");
	const float AttackDamage1 = 35.f;
	
	const FName Attack2 = FName("Swing2");
	const float AttackDamage2 = 31.f;
	
	const FName Attack3 = FName("Swing3");
	const float AttackDamage3 = 40.f;
	
	FName LastAttack = FName("");
	
	FTimerHandle AttackDelay;

	FTimerHandle TraceTimerHandle;

	bool InTargetRange(AActor* Target, double Radius);
	void MoveToTargetActor(AActor* Target);
	void LookAtSmooth();
	void PlayAttackMontage();
	void AttackTrace();
	void StartTrace();
	
	bool bCanAttack = true;
	
	UPROPERTY(EditDefaultsOnly)
	float AttackSpeed = 2.f;
	
	UPROPERTY(EditAnywhere)
	double CombatRadius = 400.f;

	UPROPERTY(EditAnywhere)
	double AttackRadius = 150.f;

	TArray<AActor*> IgnoreActors;
public:	

	FORCEINLINE bool GetIsAlive() const { return HealthComponent->IsAlive(); }
};
