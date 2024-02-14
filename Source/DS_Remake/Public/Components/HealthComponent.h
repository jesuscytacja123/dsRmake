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
	float Health;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes", meta=(AllowPrivateAccess="true"))
	float MaxHealth;

public:
	float ReceiveDamage(float Damage);
	float GetHealthPercent() const;
	bool IsAlive() const;
};