


#include "Character/Enemy/EnemyBoss.h"

#include "AIController.h"
#include "Character/DSCharacter.h"
#include "Components/HealthComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Navigation/PathFollowingComponent.h"
#include "Perception/PawnSensingComponent.h"

AEnemyBoss::AEnemyBoss()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);

	HealthComponent = CreateDefaultSubobject<UHealthComponent>("HealthComp");

	LockDot = CreateDefaultSubobject<UWidgetComponent>("LockDot");
	LockDot->SetupAttachment(GetRootComponent());

	GetCharacterMovement()->MaxWalkSpeed = 250.f;

	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensing");

	CollisionStart = CreateDefaultSubobject<USceneComponent>("Start");
	CollisionStart->SetupAttachment(GetMesh(), FName("HammerSocket"));
	
	EndCollision = CreateDefaultSubobject<USceneComponent>("End");
	EndCollision->SetupAttachment(GetMesh(), FName("HammerSocket"));
	
}

void AEnemyBoss::PawnSeen(APawn* Pawn)
{
	if(Cast<ADSCharacter>(Pawn))
	{
		CombatCharacter = Pawn;
	}
}

float AEnemyBoss::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	const float HealthLeft = HealthComponent->ReceiveDamage(DamageAmount);
	
	if(HealthLeft <= 0.f)
	{
		Die();
	}

	return 0.f;
}

bool AEnemyBoss::InTargetRange(AActor* Target, double Radius)
{
	if (Target == nullptr) return false;
	const double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();
	return DistanceToTarget <= Radius;
}


void AEnemyBoss::StartTrace()
{
	GetWorld()->GetTimerManager().SetTimer(TraceTimerHandle, this ,&AEnemyBoss::AttackTrace, 0.01f, true);
}

void AEnemyBoss::Die()
{
	GetCharacterMovement()->DisableMovement();
	SetLifeSpan(10.f);
	
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
}


void AEnemyBoss::AttackTrace()
{
	const FVector Start = CollisionStart->GetComponentLocation();
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
		20.f,
		static_cast<ETraceTypeQuery>(static_cast<EObjectTypeQuery>(ECollisionChannel::ECC_Pawn)),
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
			UGameplayStatics::ApplyDamage(TargetEnemy, AttackDamage1, Controller, this, UDamageType::StaticClass());
			UGameplayStatics::SpawnEmitterAtLocation(this, TargetEnemy->GetHitParticles(), Hit.ImpactPoint);
		}
		IgnoreActors.AddUnique(Hit.GetActor());
	}
	
}

void AEnemyBoss::PlayAttackMontage()
{
	if(bCanAttack == false) return;

	bCanAttack = false;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance && AttackMontage)
	{
		AnimInstance->Montage_Play(AttackMontage);
		if(LastAttack == Attack1)
		{
			AnimInstance->Montage_JumpToSection(Attack2);
			LastAttack = Attack2;
		}
		else if(LastAttack == Attack2)
		{
			AnimInstance->Montage_JumpToSection(Attack3);
			LastAttack = Attack3;
		}
		else
		{
			AnimInstance->Montage_JumpToSection(Attack1);
			LastAttack = Attack1;
		}
		
		StartTrace();
	}
	GetWorld()->GetTimerManager().SetTimer(AttackDelay, this, &AEnemyBoss::AttackReset, AttackSpeed, false);	
}


void AEnemyBoss::BeginPlay()
{
	Super::BeginPlay();

	LockDot->SetVisibility(false);
	BossController = Cast<AAIController>(GetController());

	if(HammerParticles)
	{
		UGameplayStatics::SpawnEmitterAttached(HammerParticles, GetMesh(), FName("FX_Trail_R_01"));
	}

	PawnSensingComponent->OnSeePawn.AddDynamic(this, &AEnemyBoss::AEnemyBoss::PawnSeen);
	
}

void AEnemyBoss::AttackReset()
{
	bCanAttack = true;
	IgnoreActors.Empty();
	GetWorldTimerManager().ClearTimer(TraceTimerHandle);
}

void AEnemyBoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(CombatCharacter != nullptr && Cast<ADSCharacter>(CombatCharacter))
	{
		MoveToTargetActor(CombatCharacter);
		if(InTargetRange(CombatCharacter, 300.f))
		{
			PlayAttackMontage();
		}
	}
}

void AEnemyBoss::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyBoss::MoveToTargetActor(AActor* Target)
{
	if (BossController == nullptr || Target == nullptr) return;
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);
	MoveRequest.SetAcceptanceRadius(80.f);
	BossController->MoveTo(MoveRequest);
}
