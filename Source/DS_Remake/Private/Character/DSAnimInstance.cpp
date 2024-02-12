// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/DSAnimInstance.h"

#include "Character/DSCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UDSAnimInstance::UpdateAnimationProperties(float DeltaSeconds)
{
	if(DSCharacter == nullptr)
	{
		DSCharacter = Cast<ADSCharacter>(TryGetPawnOwner());
	}

	if(DSCharacter)
	{
		FVector Velocity = DSCharacter->GetVelocity();
		Velocity.Z = 0;
		Speed = Velocity.Size();

		bIsInAir = DSCharacter->GetCharacterMovement()->IsFalling();

		if(DSCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f)
		{
			bIsAccelerating = true;
		}
		else
		{
			bIsAccelerating = false;
		}
		
		bIsSprinting = DSCharacter->GetIsSprinting();

		bIsWeaponEquipped = DSCharacter->GetWeaponEquipped();
	}
}

void UDSAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	DSCharacter = Cast<ADSCharacter>(TryGetPawnOwner());
}
