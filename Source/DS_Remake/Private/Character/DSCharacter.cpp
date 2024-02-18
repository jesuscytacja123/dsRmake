// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/DSCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Character/Enemy/EnemyBase.h"
#include "Character/Enemy/EnemyBoss.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/HealthComponent.h"
#include "Engine/CollisionProfile.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "HUD/HealthBarComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Character/Enemy/EnemyBoss.h"

// Sets default values
ADSCharacter::ADSCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 450.f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bInheritPitch = true;
	CameraBoom->bInheritRoll = true;
	CameraBoom->bInheritYaw = true;
	CameraBoom->SetRelativeRotation(FRotator(0.f,-45.f, 0.f));
	/* Weapon */
	
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	WeaponMesh->SetupAttachment(GetMesh(), FName("BackWeaponSocket"));
	
	/*End*/

	/*Health Component*/

	HealthComponent = CreateDefaultSubobject<UHealthComponent>("HealthComp");

	/*----------------*/
	
	BoxTraceStart = CreateDefaultSubobject<USceneComponent>("StartTrace");
	BoxTraceStart->SetupAttachment(WeaponMesh);
	BoxTraceEnd = CreateDefaultSubobject<USceneComponent>("EndTrace");
	BoxTraceEnd->SetupAttachment(WeaponMesh);
	
	/*Weapon Collision Box*/

	BoxCollision = CreateDefaultSubobject<UBoxComponent>("CollideBox");
	BoxCollision->SetupAttachment(WeaponMesh);
	BoxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BoxCollision->SetCollisionResponseToAllChannels(ECR_Overlap);
	BoxCollision->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
	/*End*/

	
	/*Character Mesh Collision Presets*/
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	
	/*-----------------------------*/
	
	FollowCamera = CreateDefaultSubobject<UCameraComponent>("FollowCamera");
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	
	// when game begins we WANT TO orient rotation to movement, but locked on target we do not
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	
	///////////////////////////
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	GetCharacterMovement()->MaxWalkSpeed = 350.f;
	
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

}

// Called when the game starts or when spawned
void ADSCharacter::BeginPlay()
{
	Super::BeginPlay();

}

void ADSCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();

	LeftRight = MovementVector.X;
	ForwardBackward = MovementVector.Y;
	
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0.f,Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(ForwardDirection, MovementVector.Y);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void ADSCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();
	AddControllerPitchInput(LookAxisVector.Y);
	AddControllerYawInput(LookAxisVector.X);
}

void ADSCharacter::Sprint()
{
	if(!HealthComponent->HasStamina()) return;
	bIsSprinting = true;
	if(bTargetLocked)
	{
		bUseControllerRotationYaw = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;
	}
	GetCharacterMovement()->MaxWalkSpeed = 550.f;
	
}

void ADSCharacter::EndSprint()
{
	bIsSprinting = false;
	if(bTargetLocked)
	{
		bUseControllerRotationYaw = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;
	}
	GetCharacterMovement()->MaxWalkSpeed = 450.f;
}

void ADSCharacter::EquipOrUnEquip()
{
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	if(bEquippedWeapon)
	{
		WeaponMesh->AttachToComponent(GetMesh(), TransformRules,FName("BackWeaponSocket"));
		DisableWeaponCollision();
		bEquippedWeapon = false;
	}
	else
	{
		WeaponMesh->AttachToComponent(GetMesh(), TransformRules, FName("Weapon"));
		WeaponMesh->AddLocalOffset(FVector(0.f,-40.f,0.f));
		DisableWeaponCollision();
		bEquippedWeapon = true;
	}
}

void ADSCharacter::Equip()
{
	
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance != nullptr && EquipMontage != nullptr)
	{
		if(!bEquippedWeapon)
		{
			AnimInstance->Montage_Play(EquipMontage);
            AnimInstance->Montage_JumpToSection(FName("Equip"), EquipMontage);
			
			GetWorld()->GetTimerManager().SetTimer(DelayAnim, this, &ADSCharacter::EquipOrUnEquip,  0.3f, false);
		}
		else
		{
			AnimInstance->Montage_Play(EquipMontage);
            AnimInstance->Montage_JumpToSection(FName("UnEquip"), EquipMontage);
            
			GetWorld()->GetTimerManager().SetTimer(DelayAnim, this, &ADSCharacter::EquipOrUnEquip, 0.73f, false);
		}
	}
}

void ADSCharacter::AttackReset()
{
	bHeavyAttack = false;
	bCanAttack = true;
	IgnoreActors.Empty();
	if(bWantsToAttack)
	{
		bWantsToAttack = false;
		DisableWeaponCollision();
		
		Attack();
	}
}

void ADSCharacter::Attack()
{
	if(HealthComponent->HasStamina() && !HealthComponent->HasStaminaToAttack()) return;
	
	if(bCanAttack == false)
	{
		bWantsToAttack = true;		
	}
	if(bIsRolling == true)
	{		
		bWantsToAttack = true;
	}
	if(bEquippedWeapon && bCanAttack && !bIsRolling)
	{
		bCanAttack = false;
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if(AnimInstance && HeavyAttackMontage && bIsSprinting)
		{
			
			HealthComponent->DecreaseStamina(HeavyAttackCost);

			
			FName SectionName = FName("");
			if(bIsSprinting && GetMovementComponent()->Velocity.Length() == 0.f)
			{
				SectionName = FName("Heavy_Start");
			}
			else if (bIsSprinting && GetMovementComponent()->Velocity.Length() > 0.f)
			{
				SectionName = FName("Heavy_2");
			}
			
			AnimInstance->Montage_Play(HeavyAttackMontage);
			AnimInstance->Montage_JumpToSection(SectionName, HeavyAttackMontage);
			StartTrace();
			bHeavyAttack = true;
			GetWorld()->GetTimerManager().SetTimer(AttackDelay, this, &ADSCharacter::AttackReset, 1.5f, false);	
			return;
		}
		if(AnimInstance && AttackMontage)
		{
			HealthComponent->DecreaseStamina(AttackCost);
			int32 RandNum = FMath::RandRange(0, (AttacksArray.Num() - 1) );
			
			/*
			 * Lowers chance to 1 attack occur 2 times in row
			 */
			if(AttacksArray[RandNum] == LastAttack)
			{
				RandNum = FMath::RandRange(0, (AttacksArray.Num() - 1) );
			}
			
			AnimInstance->Montage_Play(AttackMontage);
			AnimInstance->Montage_JumpToSection(AttacksArray[RandNum], AttackMontage);
			StartTrace();
			LastAttack = AttacksArray[RandNum];
		}
		
		GetWorld()->GetTimerManager().SetTimer(AttackDelay, this, &ADSCharacter::AttackReset, AttackSpeed, false);	
	}
}

void ADSCharacter::LockTarget()
{
	
	if(Target == nullptr && !bTargetLocked)
	{
		TArray<AActor*> Actors;
		
		const FVector Start = FollowCamera->GetComponentLocation();
		
		const FVector End = Start + (FollowCamera->GetForwardVector() * DistanceToLock);
		TArray<TEnumAsByte<EObjectTypeQuery>> Types;
		Types.Add(TArray<TEnumAsByte<EObjectTypeQuery>>::ElementType(ECollisionChannel::ECC_Pawn));
		TArray<AActor*> ToIgnore;
		ToIgnore.Add(this);
		
		FHitResult Hit;
		UKismetSystemLibrary::SphereTraceSingleForObjects(
		this,
		Start,
		End,
		100.f,
		Types,
		false,
		ToIgnore,
		EDrawDebugTrace::ForDuration,
		Hit,
		true
		);
		
		if(Hit.bBlockingHit)
		{
			if(AEnemyBase* Enemy = Cast<AEnemyBase>(Hit.GetActor()))
			{
				Target = Hit.GetActor();
			}
			if(	AEnemyBoss* Boss = Cast<AEnemyBoss>(Hit.GetActor()))
			{
				EnemyBossLockedSignature.Broadcast(Boss);
				Target = Hit.GetActor();
			}
		}
		
		if(Target != nullptr)
		{
			if(AEnemyBase* Enemy = Cast<AEnemyBase>(Hit.GetActor()))
			{
				Enemy->LockDot->SetVisibility(true);
			}
		
			if(	AEnemyBoss* Boss = Cast<AEnemyBoss>(Hit.GetActor()))
			{
				Boss->LockDot->SetVisibility(true);
			}
			
			bTargetLocked = true;
			
			bUseControllerRotationYaw = true;
			CameraBoom->bInheritPitch = false;
			GetCharacterMovement()->bOrientRotationToMovement = false;
		}
		
	}
	else
	{
		if(AEnemyBase* Enemy = Cast<AEnemyBase>(Target))
		{
			Enemy->LockDot->SetVisibility(false);
		}
		if(	AEnemyBoss* Boss = Cast<AEnemyBoss>(Target))
		{
			Boss->LockDot->SetVisibility(false);
		}
		Target = nullptr;
		bTargetLocked = false;

		bUseControllerRotationYaw = false;
		CameraBoom->bInheritPitch = true;
		GetCharacterMovement()->bOrientRotationToMovement = true;
	}
}

// Called every frame
void ADSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if(!bIsRolling && bCanAttack && !bWantsToAttack && !bIsSprinting)
	{
		if(GetVelocity().Length() == 0.f)
		{
			HealthComponent->DecreaseStamina(-1.f);
		}
		else
		{
			HealthComponent->DecreaseStamina(-.1f);
		}
	}
	
	if(bIsSprinting && GetMovementComponent()->Velocity.Length() > 0.f)
	{
		HealthComponent->DecreaseStamina(.5f);
		if(!HealthComponent->HasStamina())
		{
			EndSprint();
		}
	}
	
	LookAtSmooth();
	
}

void ADSCharacter::LookAtSmooth()
{
	
		
	if(Target != nullptr && bTargetLocked && !GetCharacterMovement()->IsFalling() && bCanAttack)
	{
		const AEnemyBase* EnemyTarget = Cast<AEnemyBase>(Target);
		const AEnemyBoss* EnemyBoss = Cast<AEnemyBoss>(Target);
		if(EnemyTarget && !EnemyTarget->GetIsAlive())
		{
			EnemyTarget->LockDot->SetVisibility(false);
			EnemyTarget->HealthBarComponent->SetVisibility(false);
			Target = nullptr;
			bTargetLocked = false;
			bUseControllerRotationYaw = false;
			CameraBoom->bInheritPitch = true;
			GetCharacterMovement()->bOrientRotationToMovement = true;
			
			return;
		}
		if(EnemyBoss && !EnemyBoss->GetIsAlive())
		{
			EnemyBoss->LockDot->SetVisibility(false);
			//EnemyBoss->HealthBarComponent->SetVisibility(false);
			Target = nullptr;
			bTargetLocked = false;
			bUseControllerRotationYaw = false;
			CameraBoom->bInheritPitch = true;
			GetCharacterMovement()->bOrientRotationToMovement = true;

			return;
		}

		const FVector ActorLoc = GetActorLocation();
		const FVector TargetLoc = Target->GetActorLocation();
		const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(ActorLoc, TargetLoc);
		
		const FRotator SmoothedRotation = FMath::Lerp(GetActorRotation(), LookAtRotation, 10.f * GetWorld()->DeltaTimeSeconds);
		Controller->SetControlRotation(SmoothedRotation);
		
		if((TargetLoc - ActorLoc).Length() > 2 * DistanceToLock)
		{
			Target = nullptr;
			bTargetLocked = false;
			bUseControllerRotationYaw = false;
			CameraBoom->bInheritPitch = true;
			GetCharacterMovement()->bOrientRotationToMovement = true;
		}
		
	}
}

// Called to bind functionality to input
void ADSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if(APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if(UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DSContext, 0);
		}
	}

	if(UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this, &ADSCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ADSCharacter::Look);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &ADSCharacter::Sprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ADSCharacter::EndSprint);
		EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Started, this, &ADSCharacter::Equip);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &ADSCharacter::Attack);
		EnhancedInputComponent->BindAction(LockAction,ETriggerEvent::Started, this, &ADSCharacter::LockTarget);
	}
}

void ADSCharacter::EnableWeaponCollision()
{
	BoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ADSCharacter::DisableWeaponCollision()
{
	BoxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	IgnoreActors.Empty();
	GetWorld()->GetTimerManager().ClearTimer(TraceTimerHandle);
}

void ADSCharacter::EnableRollCollision()
{
	GetWeaponMesh()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->SetGenerateOverlapEvents(true);
}

void ADSCharacter::DisableRollCollision()
{
	GetWeaponMesh()->SetGenerateOverlapEvents(false);
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);
}

bool ADSCharacter::GetIsDead()
{
	return bDead;
}

void ADSCharacter::StartTrace()
{
	GetWorld()->GetTimerManager().SetTimer(TraceTimerHandle, this ,&ADSCharacter::AttackTrace, 0.01f, true);
}

void ADSCharacter::AttackTrace()
{
	const FVector Start = BoxTraceStart->GetComponentLocation();
	const FVector End = BoxTraceEnd->GetComponentLocation();

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	for(AActor* Actor : IgnoreActors)
	{
		ActorsToIgnore.AddUnique(Actor);
	}
	
	FHitResult Hit;
	
	
	UKismetSystemLibrary::SphereTraceSingle(
		this,
		Start,
		End,
		12.f,
		static_cast<ETraceTypeQuery>(static_cast<EObjectTypeQuery>(ECollisionChannel::ECC_Pawn)),
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		Hit,
		true
		);

	
		if(Hit.GetActor())
		{
			AEnemyBase* TargetEnemy = Cast<AEnemyBase>(Hit.GetActor());
			if(TargetEnemy)
			{
				
				TargetEnemy->DirectionalHitReact(Hit.ImpactPoint);
				
				
				if(bHeavyAttack)
				{
					UGameplayStatics::ApplyDamage(TargetEnemy, HeavyAttackDamage, Controller, this, UDamageType::StaticClass());
				}
				else
				{
					UGameplayStatics::ApplyDamage(TargetEnemy, Damage, Controller, this, UDamageType::StaticClass());
				}
				// Heal's character on dealing damage to enemy's
				HealthComponent->ReceiveDamage(-10.f);
				UGameplayStatics::SpawnEmitterAtLocation(this, TargetEnemy->GetHitParticles(), Hit.ImpactPoint);
			}
			IgnoreActors.AddUnique(Hit.GetActor());
		}
	
}

void ADSCharacter::Die()
{
	bDead = true;
	SetLifeSpan(10.f);
	WeaponMesh->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	BoxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetSimulatePhysics(true);
	WeaponMesh->SetEnableGravity(true);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
}

void ADSCharacter::GetHit()
{
	if(HealthComponent->ReceiveDamage(Damage) <= 0.f)
	{
		Die();
	}
}

float ADSCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	float HealthLeft = HealthComponent->ReceiveDamage(DamageAmount);
	
	if(HealthLeft <= 0.f)
	{
		Die();
	}

	return 0.f;
}


