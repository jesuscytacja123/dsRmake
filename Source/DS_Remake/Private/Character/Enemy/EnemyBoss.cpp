


#include "Character/Enemy/EnemyBoss.h"

#include "AIController.h"
#include "Character/DSCharacter.h"
#include "Components/HealthComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
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
}


void AEnemyBoss::PawnSeen(APawn* Pawn)
{
	if(Cast<ADSCharacter>(Pawn))
	{
		CombatCharacter = Pawn;
	}
}

void AEnemyBoss::BeginPlay()
{
	Super::BeginPlay();

	LockDot->SetVisibility(false);
	BossController = Cast<AAIController>(GetController());

	PawnSensingComponent->OnSeePawn.AddDynamic(this, &AEnemyBoss::AEnemyBoss::PawnSeen);
	
}


void AEnemyBoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(CombatCharacter != nullptr && Cast<ADSCharacter>(CombatCharacter))
	{
		MoveToTargetActor(CombatCharacter);
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
	MoveRequest.SetAcceptanceRadius(50.f);
	BossController->MoveTo(MoveRequest);
}

