// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactables/Interactable.h"
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

	virtual void Interacted() override;

	virtual void CastToPlayer() override;

	virtual void SoftReset() override;

private:
};