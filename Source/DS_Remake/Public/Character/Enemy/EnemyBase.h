// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyBase.generated.h"

class UPawnSensingComponent;
class UHealthBarComponent;
class UWidgetComponent;
class UEnemyHealthBar;
class AAIController;
class UHealthComponent;

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	EES_Patrolling UMETA(DisplayName = "Patrolling"),
	EES_Chasing UMETA(DisplayName = "Chasing"),
	EES_Attacking UMETA(DisplayName = "Attacking")
};

UCLASS()
class DS_REMAKE_API AEnemyBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyBase();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	AAIController* EnemyController;

	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarComponent;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UWidgetComponent> LockDot;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UStaticMeshComponent> WeaponMesh;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USceneComponent> StartCollision;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USceneComponent> EndCollision;

	void CheckPatrolTarget();
	void CheckCombatTarget();

	void PlayAttackMontage();

	TArray<AActor*> IgnoreActors;

	UPROPERTY(EditDefaultsOnly, Category="Combat")
	float Damage = 21.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Combat")
	UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category="HitReact")
	UAnimMontage* HeavyHitMontage;

	UPROPERTY(EditDefaultsOnly, Category="Combat")
	TArray<FName> AttackNames;

	FName LastAttack;

	bool bIsAttacking;

	void StartTrace();

	FTimerHandle TraceTimerHandle;

	void AttackTrace();

	UFUNCTION(BlueprintCallable)
	void EnableWeaponCollision();

	UFUNCTION(BlueprintCallable)
	void DisableWeaponCollision();

	void DirectionalHitReact(const FVector& ImpactPoint);

	void PlayHeavyHitReactMontage();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	UParticleSystem* Particles;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UHealthComponent* HealthComponent;

	void Die();
	
	UPROPERTY()
	AActor* CombatTarget;

	UPROPERTY(EditAnywhere)
	double CombatRadius = 400.f;

	UPROPERTY(EditAnywhere)
	double AttackRadius = 150.f;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	AActor* PatrolTarget;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets;

	UPROPERTY(EditAnywhere)
	double PatrolRadius = 200.f;

	FTimerHandle PatrolTimer;
	
	void PatrolTimerFinished();

	bool InTargetRange(AActor* Target, double Radius);
	AActor* ChoosePatrolTarget();

	float WaitMin = 5.f; 

	float WaitMax = 10.f;

	UPROPERTY(VisibleAnywhere)
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;

	FTimerHandle AttackDelay;

	void AttackReset();

	UPROPERTY(EditDefaultsOnly)
	float AttackSpeed = 2.f;

	bool bCanAttack = true;

	void LookAtSmooth();

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* HitReactMontage;

	void PlayHitReactMontage(const FName& SectionName);

private:
	UPROPERTY(EditDefaultsOnly)
	UPawnSensingComponent* PawnSensing;

	UFUNCTION()
	void OnPawnSeen(APawn* Pawn);
	
	void MoveToTargetActor(AActor* Target);

	void LockOnTarget();

	bool bLockOnCharacter = false;
	
public:	

	UFUNCTION(BlueprintCallable)
	float GetGroundVelocity();

	FORCEINLINE UParticleSystem* GetHitParticles() const {return Particles; }

	bool GetIsAlive() const;
};
