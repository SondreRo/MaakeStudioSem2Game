// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "SmallPainting.generated.h"

/**
 * 
 */
UCLASS()
class MAAKESTUDIO_API ASmallPainting : public AInteractable
{
	GENERATED_BODY()

public:
	ASmallPainting();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	void Interacted() override;

	UFUNCTION()
	void SoftReset();
private:
	FVector SpawnLocation;
};
