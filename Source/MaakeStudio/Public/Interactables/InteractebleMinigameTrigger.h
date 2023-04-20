// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactables/Interactable.h"
#include "InteractebleMinigameTrigger.generated.h"

/**
 * 
 */
UCLASS()
class MAAKESTUDIO_API AInteractebleMinigameTrigger : public AInteractable
{
	GENERATED_BODY()

	AInteractebleMinigameTrigger();
	
	virtual void Interacted() override;


private:
	APlayerController* PlayerController;

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	class AMiniGamePawn* MinigameToPosses;
};
