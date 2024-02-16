// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyBoss.generated.h"

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

protected:
	
	virtual void BeginPlay() override;

	TObjectPtr<AAIController> BossController;

	TObjectPtr<UHealthComponent> HealthComponent;

private:

	
public:	

};
