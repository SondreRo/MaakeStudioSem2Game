// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Security_Guard.h"
#include "AIController.h"

// Sets default values
ASecurity_Guard::ASecurity_Guard()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASecurity_Guard::BeginPlay()
{
	Super::BeginPlay();

	EnemyController = Cast<AAIController>(GetController());
	if (EnemyController && PatrolTarget)
	{
		MoveTo();
	}
}

// Called every frame
void ASecurity_Guard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PatrolTarget && EnemyController)
	{
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

				MoveTo();
			}
		}
	}

}

// Called to bind functionality to input
void ASecurity_Guard::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

bool ASecurity_Guard::InTargetRange(AActor* Target, double Radius)
{
	const double DistanceToTarget = (Target->GetActorLocation() - this->GetActorLocation()).Size();



	return DistanceToTarget <= Radius;
}

void ASecurity_Guard::MoveTo()
{
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(PatrolTarget);
	MoveRequest.SetAcceptanceRadius(15.f);

	FNavPathSharedPtr NavPath;

	EnemyController->MoveTo(MoveRequest, &NavPath);

	if (Playtesting)
	{
		TArray<FNavPathPoint>& PathPoints = NavPath->GetPathPoints();
		for (auto& Point : PathPoints)
		{
			const FVector& Location = Point.Location;
			DrawDebugSphere(GetWorld(), Location, 12.f, 12, FColor::Green, false, 10.f);
		}
	}
}
