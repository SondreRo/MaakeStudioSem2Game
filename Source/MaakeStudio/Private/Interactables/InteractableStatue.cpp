// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/InteractableStatue.h"
#include "PlayerCharacter.h"
#include "components/SphereComponent.h"

AInteractableStatue::AInteractableStatue()
{
	
	PrimaryActorTick.bCanEverTick = true;

	GameScore = 50;

	SphereCollider = CreateDefaultSubobject<USphereComponent>("SphereCollider");
	SphereCollider->SetupAttachment(GetRootComponent());
}

void AInteractableStatue::BeginPlay()
{
	Super::BeginPlay();

	SpawnLocation = GetActorLocation();
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
	
	CastToPlayer();
}

void AInteractableStatue::SoftReset()
{
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
}

void AInteractableStatue::CastToPlayer()
{
	AInteractable::CastToPlayer();

	Player->AddGameScore(GameScore, 2);
}
