// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyBase.generated.h"

class UHealthBarComponent;
class UWidgetComponent;
class UEnemyHealthBar;
class AAIController;
class UHealthComponent;

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
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	UParticleSystem* Particles;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UHealthComponent* HealthComponent;

	void Die();

	
public:	

	UFUNCTION(BlueprintCallable)
	float GetGroundVelocity();

	FORCEINLINE UParticleSystem* GetHitParticles() const {return Particles; }

	bool GetIsAlive() const;
};
