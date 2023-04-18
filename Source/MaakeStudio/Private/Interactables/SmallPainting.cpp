// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/SmallPainting.h"

ASmallPainting::ASmallPainting()
{
	PrimaryActorTick.bCanEverTick = true;

	GameScore = 10;
}

void ASmallPainting::BeginPlay()
{
	Super::BeginPlay();

	SpawnLocation = GetActorLocation();
}

void ASmallPainting::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASmallPainting::Interacted()
{
	AInteractable::Interacted();

	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
}

void ASmallPainting::SoftReset()
{
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
}

void ASmallPainting::CastToPlayer()
{
	AInteractable::CastToPlayer();

}
