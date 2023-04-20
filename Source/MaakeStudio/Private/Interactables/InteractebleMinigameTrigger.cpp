// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/InteractebleMinigameTrigger.h"

#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Minigame/MiniGamePawn.h"


AInteractebleMinigameTrigger::AInteractebleMinigameTrigger()
{
	BoxCollider = CreateDefaultSubobject<UBoxComponent>("BoxCollider");
	BoxCollider->SetupAttachment(GetRootComponent());
}

void AInteractebleMinigameTrigger::Interacted()
{
	Super::Interacted();


	 if (!MinigameToPosses)
	 {
		GEngine->AddOnScreenDebugMessage(-1,5,FColor::Red,TEXT("No Minigame selected"));	
		 return;
	 }
	PlayerController = UGameplayStatics::GetPlayerController(GetWorld(),0);
	if (!PlayerController)
	{
		return;
	}

	SetActorEnableCollision(false);
	
	PlayerController->Possess(MinigameToPosses);
}
