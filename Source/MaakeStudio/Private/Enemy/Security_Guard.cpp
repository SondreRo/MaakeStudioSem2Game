// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Security_Guard.h"
#include "AIController.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "EngineUtils.h"

// Sets default values
ASecurity_Guard::ASecurity_Guard()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TargetSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("TargetSensing"));
	TargetSensing->SightRadius = 4000.f;
	TargetSensing->SetPeripheralVisionAngle(45.f);
	TargetSensing->bOnlySensePlayers = false;

	EnemyState = EEnemyState::Patrolling;

	PatrolTargetNumber = 0;
	PatrolRadius = 100;
	WaitTimer = 0;
	TotalWaitTime = 4;
	AggroTime = 0;
	TotalAggroTime = 2;
	WalkSpeed = 200;
	ChaseSpeed = 400;
}

// Called when the game starts or when spawned
void ASecurity_Guard::BeginPlay()
{
	Super::BeginPlay();

	EnemyController = Cast<AAIController>(GetController());

	if (TargetSensing)
	{
		TargetSensing->OnSeePawn.AddDynamic(this, &ASecurity_Guard::TargetSeen);
	}

	PatrolTarget = PatrolTargets[0];
	if (PatrolTarget)
	{
		MoveTo(PatrolTarget);
	}
}

// Called every frame
void ASecurity_Guard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (EnemyState == EEnemyState::Patrolling)
	{
		RandomPatrolling == true ? MoveToRandomPoint() : MoveToPoint();
	}
	else if (EnemyState == EEnemyState::Chasing)
	{
		AggroTimer(DeltaTime);
	}
	else
	{
		PauseWhenFinished(DeltaTime);
	}
}

// Called to bind functionality to input
void ASecurity_Guard::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

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

bool ASecurity_Guard::InTargetRange(AActor* Target, double Radius)
{
	const double DistanceToTarget = (Target->GetActorLocation() - this->GetActorLocation()).Size();

	return DistanceToTarget <= Radius;
}

void ASecurity_Guard::MoveTo(AActor* Target)
{
	if (Target == nullptr || EnemyController == nullptr)
	{
		return;
	}

	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);
	MoveRequest.SetAcceptanceRadius(15.f);

	FNavPathSharedPtr NavPath;

	EnemyController->MoveTo(MoveRequest, &NavPath);

	if (PlayTest)
	{
		TArray<FNavPathPoint>& PathPoints = NavPath->GetPathPoints();
		for (auto& Point : PathPoints)
		{
			const FVector& Location = Point.Location;
			DrawDebugSphere(GetWorld(), Location, 12.f, 12, FColor::Green, false, 10.f);
		}
	}
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
		UE_LOG(LogTemp, Warning, TEXT("Target Escaped!"))
		MoveTo(PatrolTarget);
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

void ASecurity_Guard::TargetSeen(APawn* Target)
{
	if (Target->ActorHasTag("test"))
	{
		ChasingTarget(Target);
	}
}
