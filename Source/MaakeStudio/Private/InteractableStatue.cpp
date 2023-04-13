// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableStatue.h"

void AInteractableStatue::Interacted()
{
	//AInteractable::Interacted();

	GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, TEXT("I AM SLAVE"));

}

