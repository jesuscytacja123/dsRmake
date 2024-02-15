// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/HealthComponent.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

float UHealthComponent::ReceiveDamage(float Damage)
{
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);
	return Health;
}

float UHealthComponent::DecreaseStamina(float StaminaToDecrease)
{
	Stamina = FMath::Clamp(Stamina - StaminaToDecrease, 0.f, MaxStamina);
	return Stamina;
}

float UHealthComponent::GetStaminaPercent() const
{
	return Stamina / MaxStamina;
}

float UHealthComponent::GetHealthPercent() const
{
	return Health / MaxHealth;
}

bool UHealthComponent::IsAlive() const
{
	return Health > 0.f;
}

bool UHealthComponent::HasStamina() const
{
	return Stamina > 0.f;
}

bool UHealthComponent::HasStaminaToAttack() const
{
	return Stamina > 25.f;
}

bool UHealthComponent::HasStaminoToRoll() const
{
	return Stamina > 30.f;
}

void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}
