// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Enemy/EnemyBase.h"

#include "AIController.h"
#include "Character/DSCharacter.h"
#include "Components/HealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "HUD/HealthBarComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Navigation/PathFollowingComponent.h"
#include "Perception/PawnSensingComponent.h"

// Sets default values
AEnemyBase::AEnemyBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);

	HealthComponent = CreateDefaultSubobject<UHealthComponent>("HealthComp");

	HealthBarComponent = CreateDefaultSubobject<UHealthBarComponent>("HealthBarComp");
	HealthBarComponent->SetupAttachment(GetRootComponent());

	LockDot = CreateDefaultSubobject<UWidgetComponent>("LockDot");
	LockDot->SetupAttachment(GetRootComponent());

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>("Weapon");
	WeaponMesh->SetupAttachment(GetMesh(), FName("Weapon"));
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	WeaponMesh->SetCollisionResponseToAllChannels(ECR_Overlap);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	StartCollision = CreateDefaultSubobject<USceneComponent>("Start");
	StartCollision->SetupAttachment(WeaponMesh);

	EndCollision = CreateDefaultSubobject<USceneComponent>("End");
	EndCollision->SetupAttachment(WeaponMesh);

	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensingComp");

	GetCharacterMovement()->MaxWalkSpeed = 225.f;
}

void AEnemyBase::CheckPatrolTarget()
{
	if (InTargetRange(PatrolTarget, PatrolRadius))
	{
		PatrolTarget = ChoosePatrolTarget();
		const float WaitTime = FMath::RandRange(WaitMin, WaitMax);
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemyBase::PatrolTimerFinished, WaitTime);
	}
}


void AEnemyBase::CheckCombatTarget()
{
	if (!InTargetRange(CombatTarget, CombatRadius))
	{
		// Outside combat radius, lose interest
		CombatTarget = nullptr;
		EnemyState = EEnemyState::EES_Patrolling;
		GetCharacterMovement()->MaxWalkSpeed = 125.f;
		MoveToTargetActor(PatrolTarget);
		UE_LOG(LogTemp, Warning, TEXT("Lose Interest"));
	}
	else if (!InTargetRange(CombatTarget, AttackRadius) && EnemyState != EEnemyState::EES_Chasing)
	{
		// Outside attack range, chase character
		EnemyState = EEnemyState::EES_Chasing;
		GetCharacterMovement()->MaxWalkSpeed = 250.f;
		MoveToTargetActor(CombatTarget);
		UE_LOG(LogTemp, Warning, TEXT("Chase Player"));
	}
	else if (InTargetRange(CombatTarget, AttackRadius) && EnemyState != EEnemyState::EES_Attacking && !bLockOnCharacter)
	{
		// Inside attack range, attack character
		EnemyState = EEnemyState::EES_Attacking;
		
		bLockOnCharacter = true;
		
		UE_LOG(LogTemp, Warning, TEXT("Attack"));
	}
}

void AEnemyBase::StartTrace()
{
	GetWorld()->GetTimerManager().SetTimer(TraceTimerHandle, this ,&AEnemyBase::AttackTrace, 0.01f, true);
}

void AEnemyBase::DirectionalHitReact(const FVector& ImpactPoint)
{
	const FVector Forward = GetActorForwardVector();
	// Lower Impact Point to the Enemy's Actor Location Z
	const FVector ImpactLowered(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
	const FVector ToHit = (ImpactLowered - GetActorLocation()).GetSafeNormal();

	// Forward * ToHit = |Forward||ToHit| * cos(theta)
	// |Forward| = 1, |ToHit| = 1, so Forward * ToHit = cos(theta)
	const double CosTheta = FVector::DotProduct(Forward, ToHit);
	// Take the inverse cosine (arc-cosine) of cos(theta) to get theta
	double Theta = FMath::Acos(CosTheta);
	// convert from radians to degrees
	Theta = FMath::RadiansToDegrees(Theta);

	// if CrossProduct points down, Theta should be negative
	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);
	if (CrossProduct.Z < 0)
	{
		Theta *= -1.f;
	}

	FName Section("FromBack");

	if (Theta >= -45.f && Theta < 45.f)
	{
		Section = FName("FromFront");
	}

	PlayHitReactMontage(Section);
	
}

void AEnemyBase::PlayHitReactMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
	}
}

void AEnemyBase::PlayHeavyHitReactMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HeavyHitMontage)
	{
		AnimInstance->Montage_Play(HeavyHitMontage);
		AnimInstance->Montage_JumpToSection(FName("HeavyHit"), HeavyHitMontage);
	}
}


void AEnemyBase::AttackTrace()
{
	const FVector Start = StartCollision->GetComponentLocation();
	const FVector End = EndCollision->GetComponentLocation();

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
		static_cast<ETraceTypeQuery>(static_cast<EObjectTypeQuery>(ECollisionChannel::ECC_WorldDynamic)),
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		Hit,
		true
		);

	
	if(Hit.GetActor())
	{
		ADSCharacter* TargetEnemy = Cast<ADSCharacter>(Hit.GetActor());
		if(TargetEnemy && !TargetEnemy->bIsRolling)
		{
			UGameplayStatics::ApplyDamage(TargetEnemy, Damage, Controller, this, UDamageType::StaticClass());
			UGameplayStatics::SpawnEmitterAtLocation(this, TargetEnemy->GetHitParticles(), Hit.ImpactPoint);
		}
		IgnoreActors.AddUnique(Hit.GetActor());
	}
	
}

void AEnemyBase::EnableWeaponCollision()
{
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemyBase::DisableWeaponCollision()
{
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	IgnoreActors.Empty();
	GetWorld()->GetTimerManager().ClearTimer(TraceTimerHandle);
}


void AEnemyBase::PlayAttackMontage()
{
	if(bCanAttack == false) return;

	bCanAttack = false;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance && AttackMontage)
	{
		int32 RandNum = FMath::RandRange(0, (AttackNames.Num() - 1) );
			
		/*
		 * Lowers chance to 1 attack occur 2 times in row
		 */
		if(AttackNames[RandNum] == LastAttack)
		{
			RandNum = FMath::RandRange(0, (AttackNames.Num() - 1) );
		}
			
		AnimInstance->Montage_Play(AttackMontage);
		AnimInstance->Montage_JumpToSection(AttackNames[RandNum], AttackMontage);
		StartTrace();
		LastAttack = AttackNames[RandNum];
	}
	GetWorld()->GetTimerManager().SetTimer(AttackDelay, this, &AEnemyBase::AttackReset, AttackSpeed, false);	
}

// Called when the game starts or when spawned
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	if(HealthBarComponent)
	{
		HealthBarComponent->SetHealthPercent(HealthComponent->GetHealthPercent());
	}
	LockDot->SetVisibility(false);
	EnemyController = Cast<AAIController>(GetController());

	MoveToTargetActor(PatrolTarget);
	
	if(PawnSensing)
	{
		PawnSensing->OnSeePawn.AddDynamic(this, &AEnemyBase::OnPawnSeen);
	}
	
}

void AEnemyBase::AttackReset()
{
	bCanAttack = true;
}

void AEnemyBase::LookAtSmooth()
{
	if(CombatTarget != nullptr && bLockOnCharacter && !GetCharacterMovement()->IsFalling() && bCanAttack)
	{
		ADSCharacter* EnemyTarget = Cast<ADSCharacter>(CombatTarget);
		
		if(!EnemyTarget->GetHealthComponent()->IsAlive())
		{
			CombatTarget = nullptr;
			bLockOnCharacter = false;
			bUseControllerRotationYaw = false;
			GetCharacterMovement()->bOrientRotationToMovement = true;
	
			return;
		}
		const FVector ActorLoc = GetActorLocation();
		const FVector TargetLoc = CombatTarget->GetActorLocation();
		const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(ActorLoc, TargetLoc);
		
		const FRotator SmoothedRotation = FMath::Lerp(GetActorRotation(), LookAtRotation, 10.f * GetWorld()->DeltaTimeSeconds);
		EnemyController->SetControlRotation(SmoothedRotation);
		
		if((TargetLoc - ActorLoc).Length() > 2 * CombatRadius)
		{
			CombatTarget = nullptr;
			bLockOnCharacter = false;
			bUseControllerRotationYaw = false;
			GetCharacterMovement()->bOrientRotationToMovement = true;
		}
	}
}


void AEnemyBase::OnPawnSeen(APawn* Pawn)
{
	if(ADSCharacter* DSChar = Cast<ADSCharacter>(Pawn))
	{
		GetWorldTimerManager().ClearTimer(PatrolTimer);
		GetCharacterMovement()->MaxWalkSpeed = 300.f;
		CombatTarget = Pawn;
		
		if (EnemyState != EEnemyState::EES_Attacking)
		{
			EnemyState = EEnemyState::EES_Chasing;
			MoveToTargetActor(CombatTarget);
			
			bLockOnCharacter = true;
			
			UE_LOG(LogTemp, Warning, TEXT("Pawn Seen, Chase Player"));
		}
		
	}
}


AActor* AEnemyBase::ChoosePatrolTarget()
{
	TArray<AActor*> ValidTargets;
	for (AActor* Target : PatrolTargets)
	{
		if (Target != PatrolTarget)
		{
			ValidTargets.AddUnique(Target);
		}
	}

	const int32 NumPatrolTargets = ValidTargets.Num();
	if (NumPatrolTargets > 0)
	{
		const int32 TargetSelection = FMath::RandRange(0, NumPatrolTargets - 1);
		return ValidTargets[TargetSelection];
	}
	return nullptr;
}


void AEnemyBase::PatrolTimerFinished()
{
	MoveToTargetActor(PatrolTarget);
}

bool AEnemyBase::InTargetRange(AActor* Target, double Radius)
{
	if (Target == nullptr) return false;
	const double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();
	return DistanceToTarget <= Radius;
}

void AEnemyBase::MoveToTargetActor(AActor* Target)
{
	if (EnemyController == nullptr || Target == nullptr) return;
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);
	MoveRequest.SetAcceptanceRadius(50.f);
	EnemyController->MoveTo(MoveRequest);
}

void AEnemyBase::LockOnTarget()
{
	if(InTargetRange(CombatTarget, AttackRadius))
	{
		PlayAttackMontage();
	}
	else
	{
		MoveToTargetActor(CombatTarget);
	}
}


// Called every frame
void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(!bLockOnCharacter)
	{
		if (EnemyState > EEnemyState::EES_Patrolling)
		{
			CheckCombatTarget();
		}
		else
		{
			CheckPatrolTarget();
		}
	}
	else
	{
		LockOnTarget();
	}
	LookAtSmooth();
}

// Called to bind functionality to input
void AEnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyBase::Die()
{
	GetCharacterMovement()->StopMovementImmediately();
	
	SetLifeSpan(10.f);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);

	WeaponMesh->SetSimulatePhysics(true);
	WeaponMesh->SetEnableGravity(true);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	WeaponMesh->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	
}

float AEnemyBase::GetGroundVelocity()
{
	return GetMovementComponent()->Velocity.Length();
}

bool AEnemyBase::GetIsAlive() const
{
	return HealthComponent->IsAlive();
}

float AEnemyBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
                             AActor* DamageCauser)
{
	const float HealthLeft = HealthComponent->ReceiveDamage(DamageAmount);

	if(HealthBarComponent)
	{
		HealthBarComponent->SetHealthPercent(HealthComponent->GetHealthPercent());
	}
	if(ADSCharacter* Attacker = Cast<ADSCharacter>(DamageCauser))
	{
		CombatTarget = DamageCauser;
		EnemyState = EEnemyState::EES_Chasing;
	}
	
	if(HealthLeft <= 0.f)
	{
		Die();
	}

	return 0.f;
}

