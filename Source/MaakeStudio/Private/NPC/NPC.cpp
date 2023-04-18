// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC/NPC.h"
#include "AIController.h"

// Sets default values
ANPC::ANPC()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WaitTime = 0;
	TotalWaitTime = 2;
	WalkRadius = 300;
}

// Called when the game starts or when spawned
void ANPC::BeginPlay()
{
	Super::BeginPlay();

	NPCController = Cast<AAIController>(GetController());

	const int32 TargetSelection = FMath::RandRange(0, WalkTargets.Num() - 1);
	WalkTarget = WalkTargets[TargetSelection];
	if (WalkTarget != nullptr)
	{
		NPCState = ENPCState::Walking;
		MoveTo(WalkTarget);
	}
}

// Called every frame
void ANPC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (NPCState == ENPCState::Walking)
	{
		MoveToRandomPoint();
	}
	else if (NPCState == ENPCState::Waiting)
	{
		WaitTimer(DeltaTime);
	}
}

// Called to bind functionality to input
void ANPC::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ANPC::MoveToRandomPoint()
{
	if (WalkTargets.Num() < 2)
	{
		return;
	}

	if (InTargetRange(WalkTarget, WalkRadius))
	{
		TArray<AActor*> ValidTargets;
		for (AActor* Target : WalkTargets)
		{
			if (Target != WalkTarget)
			{
				ValidTargets.AddUnique(Target);
			}
		}

		const int32 NumPatrolTargets = ValidTargets.Num();
		if (NumPatrolTargets > 0)
		{
			const int32 TargetSelection = FMath::RandRange(0, NumPatrolTargets - 1);
			AActor* Target = ValidTargets[TargetSelection];
			WalkTarget = Target;

			NPCState = ENPCState::Waiting;
		}
	}
}

void ANPC::MoveTo(AActor* Target)
{
	if (Target == nullptr || NPCController == nullptr)
	{
		return;
	}

	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);
	MoveRequest.SetAcceptanceRadius(200.f);

	FNavPathSharedPtr NavPath;

	NPCController->MoveTo(MoveRequest, &NavPath);

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

bool ANPC::InTargetRange(AActor* Target, double Radius)
{
	if (Target == nullptr)
	{
		return false;
	}

	const double DistanceToTarget = (Target->GetActorLocation() - this->GetActorLocation()).Size();

	return DistanceToTarget <= Radius;
}

void ANPC::WaitTimer(float DeltaTime)
{
	WaitTime += DeltaTime;

	if (WaitTime >= TotalWaitTime)
	{
		WaitTime = 0;
		NPCState = ENPCState::Walking;
		MoveTo(WalkTarget);
	}
}

void ANPC::CheckStandingStill(float DeltaTime)
{
	StandingStillTime += DeltaTime;

	if (StandingStillTime >= TotalStillTime)
	{
		StandingStillTime = 0;
		const int32 TargetSelection = FMath::RandRange(0, WalkTargets.Num() - 1);
		WalkTarget = WalkTargets[TargetSelection];
		MoveTo(WalkTarget);
	}
}

void ANPC::SoftReset()
{
	
}
