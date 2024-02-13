// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/DSCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/HealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ADSCharacter::ADSCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 450.f;
	CameraBoom->bUsePawnControlRotation = true;

	/* Weapon */
	
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	WeaponMesh->SetupAttachment(GetMesh(), FName("BackWeaponSocket"));
	
	/*End*/

	/* Health Component */

	
	/*-------------------*/
	
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

	GetCharacterMovement()->MaxWalkSpeed = 450.f;
	
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

}



// Called when the game starts or when spawned
void ADSCharacter::BeginPlay()
{
	Super::BeginPlay();

	CharactersCurrentHealth = CharactersMaxHealth;
	
}


void ADSCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();
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
	bIsSprinting = true;
	/*bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;*/
	GetCharacterMovement()->MaxWalkSpeed = 550.f;
}

void ADSCharacter::EndSprint()
{
	bIsSprinting = false;
	/*bUseControllerRotationYaw = true;
	GetCharacterMovement()->bOrientRotationToMovement = true;*/
	GetCharacterMovement()->MaxWalkSpeed = 450.f;
}

void ADSCharacter::EquipOrUnEquip()
{
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	if(bEquippedWeapon)
	{
		WeaponMesh->AttachToComponent(GetMesh(), TransformRules,FName("BackWeaponSocket"));
		bEquippedWeapon = false;
	}
	else
	{
		WeaponMesh->AttachToComponent(GetMesh(), TransformRules, FName("Weapon"));
		WeaponMesh->AddLocalOffset(FVector(0.f,-40.f,0.f));
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
	bCanAttack = true;
	IgnoreActors.Empty();
}


void ADSCharacter::Attack()
{
	
	if(bEquippedWeapon && bCanAttack)
	{
		bCanAttack = false;
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if(AnimInstance && AttackMontage)
		{
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

// Called every frame
void ADSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &ADSCharacter::Sprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ADSCharacter::EndSprint);
		EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Started, this, &ADSCharacter::Equip);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &ADSCharacter::Attack);
	}
}

void ADSCharacter::EnableWeaponCollision()
{
	BoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	IgnoreActors.Empty();	
}

void ADSCharacter::DisableWeaponCollision()
{
	BoxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	IgnoreActors.Empty();
	GetWorld()->GetTimerManager().ClearTimer(TraceTimerHandle);
}

void ADSCharacter::StartTrace()
{
	GetWorld()->GetTimerManager().SetTimer(TraceTimerHandle, this ,&ADSCharacter::AttackTrace, 0.1f, true);
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
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForDuration,
		Hit,
		true,
		FLinearColor::Blue,
		FLinearColor::Black,
		0.1f
		);

	
		if(Hit.GetActor())
		{
			ICombatInterface* CombatInterface = Cast<ICombatInterface>(Hit.GetActor());
			if(CombatInterface)
			{
				CombatInterface->GetHit();
			}
			IgnoreActors.AddUnique(Hit.GetActor());
		}
	
}

void ADSCharacter::Die()
{
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
	if(CharactersCurrentHealth - Damage > 0.f)
	{
		CharactersCurrentHealth = CharactersCurrentHealth - Damage;
	}
	else
	{
		Die();
	}	
	
}


