// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerSideCharacter.h"

#include "AIController.h"

// Sets default values
APlayerSideCharacter::APlayerSideCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APlayerSideCharacter::BeginPlay()
{
	Super::BeginPlay();
	PlayerSideController = Cast<AAIController>(GetController());

	Tags.Add(FName("test"));

	SpawnLocation = GetActorLocation();
	SpawnRotation = GetActorRotation();
}

// Called every frame
void APlayerSideCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlayerSideCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void APlayerSideCharacter::WalkToPoint(FVector inWalkPoint)
{
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalLocation(inWalkPoint);
	MoveRequest.SetAcceptanceRadius(15.f);
	//MoveRequest.SetCanStrafe(false);


	FNavPathSharedPtr NavPath;

	PlayerSideController->MoveTo(MoveRequest, &NavPath);

}

void APlayerSideCharacter::SoftReset()
{
	SetActorLocation(SpawnLocation);
	SetActorRotation(SpawnRotation);
	WalkToPoint(SpawnLocation);
}

