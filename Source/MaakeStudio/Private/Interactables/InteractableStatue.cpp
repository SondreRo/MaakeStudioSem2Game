// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/InteractableStatue.h"

AInteractableStatue::AInteractableStatue()
{
	PrimaryActorTick.bCanEverTick = true;

	GameScore = 50;
}

void AInteractableStatue::BeginPlay()
{
	Super::BeginPlay();

	SpawnLocation() = GetActorLocation();
}

void AInteractableStatue::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AInteractableStatue::Interacted()
{
	AInteractable::Interacted();

	GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, TEXT("I AM SLAVE"));

	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
}

void AInteractableStatue::SoftReset()
{
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
}
