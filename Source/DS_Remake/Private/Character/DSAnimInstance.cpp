// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/DSAnimInstance.h"

#include "Character/DSCharacter.h"
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

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

		bIsTargetLocked = DSCharacter->GetIsTargetLocked();

		const FRotator AimRotation = DSCharacter->GetBaseAimRotation();
		const FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(DSCharacter->GetVelocity());
		
		MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;

		
	}
}

void UDSAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	DSCharacter = Cast<ADSCharacter>(TryGetPawnOwner());
}
