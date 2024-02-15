// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DS_REMAKE_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UHealthComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;

private:
	// Current Health
	UPROPERTY(EditAnywhere, Category = "Actor Attributes", meta=(AllowPrivateAccess="true"))
	float Health = 100.f;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes", meta=(AllowPrivateAccess="true"))
	float MaxHealth  = 100.f;

	// Current Health
	UPROPERTY(EditAnywhere, Category = "Actor Attributes", meta=(AllowPrivateAccess="true"))
	float Stamina  = 150.f;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes", meta=(AllowPrivateAccess="true"))
	float MaxStamina = 150.f;
public:
	UFUNCTION(BlueprintCallable)
	float ReceiveDamage(float Damage);

	UFUNCTION(BlueprintCallable)
	float DecreaseStamina(float StaminaToDecrease);
	
	UFUNCTION(BlueprintCallable)
	float GetHealthPercent() const;

	UFUNCTION(BlueprintCallable)
	float GetStaminaPercent() const;
	
	UFUNCTION(BlueprintCallable)
	bool IsAlive() const;

	UFUNCTION(BlueprintCallable)
	bool HasStamina() const;

	UFUNCTION(BlueprintCallable)
	bool HasStaminaToAttack() const;

	UFUNCTION(BlueprintCallable)
	bool HasStaminoToRoll() const;
};