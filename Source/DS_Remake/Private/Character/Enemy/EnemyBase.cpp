// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Enemy/EnemyBase.h"

#include "AIController.h"
#include "Components/HealthComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "HUD/EnemyHealthBar.h"
#include "HUD/HealthBarComponent.h"

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

}

// Called when the game starts or when spawned
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	if(HealthBarComponent)
	{
		HealthBarComponent->SetHealthPercent(HealthComponent->GetHealthPercent());
	}
	
	EnemyController = Cast<AAIController>(Controller);
	
}

// Called every frame
void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyBase::Die()
{
	SetLifeSpan(10.f);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
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
	if(HealthLeft <= 0.f)
	{
		Die();
	}

	return 0.f;
}

