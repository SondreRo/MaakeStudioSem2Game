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
		for (int i = 0; i < PatrolTargets.Num(); i++)
		{
			FAIMoveRequest MoveRequest;
			MoveRequest.SetGoalActor(PatrolTargets[i]);
			MoveRequest.SetAcceptanceRadius(15.f);

			FNavPathSharedPtr NavPath;

			EnemyController->MoveTo(MoveRequest, &NavPath);
			TArray<FNavPathPoint>& PathPoints = NavPath->GetPathPoints();
			for (auto& Point : PathPoints)
			{
				const FVector& Location = Point.Location;
				DrawDebugSphere(GetWorld(), Location, 12.f, 12, FColor::Green, false, 10.f);
			}
		}
	}
}

// Called every frame
void ASecurity_Guard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASecurity_Guard::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

