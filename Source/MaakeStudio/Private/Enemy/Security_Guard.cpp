// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Security_Guard.h"
#include "AIController.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerCharacter.h"

// Sets default values
ASecurity_Guard::ASecurity_Guard()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TargetSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("TargetSensing"));
	TargetSensing->SightRadius = 2000.f;
	TargetSensing->SetPeripheralVisionAngle(45.f);
	TargetSensing->bOnlySensePlayers = false;

	PatrolTargetNumber = 0;
	PatrolRadius = 100;
	ChaseRadius = 200;
	CheckRadius = 100;
	WaitTimer = 0;
	TotalWaitTime = 4;
	AggroTime = 0;
	TotalAggroTime = 2;
	WalkSpeed = 200;
	ChaseSpeed = 450;
	CatchedPlayer = false;
}

// Called when the game starts or when spawned
void ASecurity_Guard::BeginPlay()
{
	Super::BeginPlay();

	EnemyController = Cast<AAIController>(GetController());
	SpawnLocation = GetActorLocation();
	EnemyState = EEnemyState::Patrolling;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	if (TargetSensing != nullptr)
	{
		TargetSensing->OnSeePawn.AddDynamic(this, &ASecurity_Guard::TargetSeen);
	}

	PatrolTarget = PatrolTargets[0];
	if (PatrolTarget != nullptr)
	{
		MoveTo(PatrolTarget);
	}

	AActor* TempActor;
	TempActor = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCharacter::StaticClass());
	PlayerCharacter = Cast<APlayerCharacter>(TempActor);
}

// Called every frame
void ASecurity_Guard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AI_TickRun(DeltaTime);
}

// Called to bind functionality to input
void ASecurity_Guard::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

//---------------------------//
//------Private Methods------//
//---------------------------//

/*-----Main Tick Methods-----*/
void ASecurity_Guard::AI_TickRun(float DeltaTime)
{
	if (EnemyState == EEnemyState::Patrolling)
	{
		RandomPatrolling == true ? MoveToRandomPoint() : MoveToPoint();
	}
	else if (EnemyState == EEnemyState::Checking)
	{
		CheckingLocation();
	}
	else if (EnemyState == EEnemyState::Chasing)
	{
		if (ChaseTarget != nullptr && InTargetRange(ChaseTarget, ChaseRadius) && CatchedPlayer == false)
		{
			PlayerCharacter->Failed();
			CatchedPlayer = true;
			Jump();
		}
		AggroTimer(DeltaTime);
	}
	else if (EnemyState == EEnemyState::Waiting)
	{
		PauseWhenFinished(DeltaTime);
	}
}

/*------Movement Methods-----*/

void ASecurity_Guard::MoveToPoint()
{
	if (InTargetRange(PatrolTarget, PatrolRadius))
	{
		PatrolTargetNumber++;

		if (PatrolTargetNumber == PatrolTargets.Num())
		{
			PatrolTargetNumber = 0;
		}

		PatrolTarget = PatrolTargets[PatrolTargetNumber];
		EnemyState = EEnemyState::Waiting;
		UE_LOG(LogTemp, Warning, TEXT("Gonna wait"))
	}
}

void ASecurity_Guard::MoveToRandomPoint()
{
	if (PatrolTargets.Num() < 2)
	{
		MoveToPoint();
		return;
	}

	if (InTargetRange(PatrolTarget, PatrolRadius))
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
			AActor* Target = ValidTargets[TargetSelection];
			PatrolTarget = Target;

			EnemyState = EEnemyState::Waiting;
		}
	}
}

void ASecurity_Guard::MoveToLocation(FVector& location)
{
	if (EnemyController == nullptr)
	{
		return;
	}

	TempLocation = location;

	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalLocation(location);
	MoveRequest.SetAcceptanceRadius(15.f);

	FNavPathSharedPtr NavPath;

	GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed;
	EnemyController->MoveTo(MoveRequest, &NavPath);
}

void ASecurity_Guard::MoveTo(AActor* Target)
{
	if (Target == nullptr || EnemyController == nullptr)
	{
		return;
	}

	TempLocation = Target->GetActorLocation();

	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);
	MoveRequest.SetAcceptanceRadius(15.f);

	FNavPathSharedPtr NavPath;

	EnemyController->MoveTo(MoveRequest, &NavPath);

	if (PlayTest == true)
	{
		TArray<FNavPathPoint>& PathPoints = NavPath->GetPathPoints();
		for (auto& Point : PathPoints)
		{
			const FVector& Location = Point.Location;
			DrawDebugSphere(GetWorld(), Location, 12.f, 12, FColor::Green, false, 10.f);
		}
	}
}

void ASecurity_Guard::ChasingTarget(APawn* Target)
{
	if (EnemyState != EEnemyState::Chasing)
	{
		UE_LOG(LogTemp, Warning, TEXT("Target Located!"))
		EnemyState = EEnemyState::Chasing;
		GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed;
		ChaseTarget = Target;
		MoveTo(ChaseTarget);
	}
	AggroTime = 0;
}

/*-------Other Methods-------*/

bool ASecurity_Guard::InTargetRange(AActor* Target, double Radius)
{
	if (Target == nullptr)
	{
		return false;
	}

	const double DistanceToTarget = (Target->GetActorLocation() - this->GetActorLocation()).Size();
	
	return FMath::Abs(DistanceToTarget) <= Radius;
}

bool ASecurity_Guard::InTargetRange(FVector& location, double Radius)
{
	const double DistanceToTarget = (location - this->GetActorLocation()).Size();

	return FMath::Abs(DistanceToTarget) <= Radius;
}

void ASecurity_Guard::PauseWhenFinished(float DeltaTime)
{
	WaitTimer += DeltaTime;

	if (WaitTimer >= TotalWaitTime)
	{
		WaitTimer = 0;
		EnemyState = EEnemyState::Patrolling;
		MoveTo(PatrolTarget);
	}
}

void ASecurity_Guard::AggroTimer(float DeltaTime)
{
	AggroTime += DeltaTime;

	if (AggroTime >= TotalAggroTime)
	{
		EnemyState = EEnemyState::Patrolling;
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
		UE_LOG(LogTemp, Warning, TEXT("Target Escaped!"))
		MoveTo(PatrolTarget);
	}
}

void ASecurity_Guard::AddPatrolTargets()
{
	for (TActorIterator<AActor> Actors(GetWorld()); Actors; ++Actors)
	{
		AActor* Actor = *Actors;

		if (Actor && Actor->GetFName().ToString() == "Patrol Points")
		{
			Actor->GetAllChildActors(TestTargets);
			break;
		}
	}
}

void ASecurity_Guard::CheckingLocation()
{
	if (InTargetRange(CheckLocation, CheckRadius))
	{
		EnemyState = EEnemyState::Waiting;
	}
}

//---------------------------//
//------Public Methods-------//
//---------------------------//

void ASecurity_Guard::TargetSeen(APawn* Target)
{
	if (Target->ActorHasTag("PlayerSideCharacter") && EnemyState != EEnemyState::Frozen || Target->ActorHasTag("Sus"))
	{
		ChasingTarget(Target);
	}
}

void ASecurity_Guard::SoftReset()
{
	SetActorLocation(SpawnLocation);
	PatrolTarget = PatrolTargets[0];
	PatrolTargetNumber = 0;
	ChaseTarget = nullptr;
	EnemyState = EEnemyState::Patrolling;
	CatchedPlayer = false;
	AggroTime = 0;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;	
	if (PatrolTarget != nullptr)
	{
		MoveTo(PatrolTarget);
	}
}

void ASecurity_Guard::SendChasingTarget(FVector& location)
{
	if (EnemyState != EEnemyState::Checking && EnemyState != EEnemyState::Frozen)
	{
		EnemyState = EEnemyState::Checking;
		CheckLocation = location;
		MoveToLocation(CheckLocation);
	}
}

void ASecurity_Guard::FreezeWhileMinigame()
{
	TempEnemyState = EnemyState;

	EnemyState = EEnemyState::Frozen;
	GetCharacterMovement()->DisableMovement();
}

void ASecurity_Guard::UnFreezeAfterMinigame()
{
	EnemyState = TempEnemyState;
	GetCharacterMovement()->SetMovementMode(MOVE_NavWalking);
	MoveToLocation(TempLocation);
}