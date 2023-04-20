// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigame/MiniGamePawn.h"

// Sets default values
AMiniGamePawn::AMiniGamePawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMiniGamePawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMiniGamePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMiniGamePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

