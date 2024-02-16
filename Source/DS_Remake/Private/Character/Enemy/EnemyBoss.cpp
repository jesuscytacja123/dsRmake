


#include "Character/Enemy/EnemyBoss.h"

#include "AIController.h"
#include "Components/HealthComponent.h"

AEnemyBoss::AEnemyBoss()
{
 	
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);

	HealthComponent = CreateDefaultSubobject<UHealthComponent>("HealthComp");
}


void AEnemyBoss::BeginPlay()
{
	Super::BeginPlay();
	
	BossController = Cast<AAIController>(GetController());
}


void AEnemyBoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemyBoss::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

