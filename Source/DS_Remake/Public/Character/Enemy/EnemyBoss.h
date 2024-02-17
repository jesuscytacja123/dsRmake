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

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UHealthComponent> HealthComponent;

	UPROPERTY()
	TObjectPtr<AActor> CombatCharacter;

	UPROPERTY(EditDefaultsOnly)
	UPawnSensingComponent* PawnSensingComponent;
private:

	UPROPERTY(EditDefaultsOnly, Category="Combat")
	UAnimMontage* AttackMontage;
	

	void MoveToTargetActor(AActor* Target);
public:	

	FORCEINLINE bool GetIsAlive() const { return HealthComponent->IsAlive(); }
};
