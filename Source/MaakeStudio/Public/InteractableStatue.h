// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "InteractableStatue.generated.h"

/**
 * 
 */
UCLASS()
class MAAKESTUDIO_API AInteractableStatue : public AInteractable
{
	GENERATED_BODY()

	

	void Interacted() override;


};
