// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/InteractebleMinigameTrigger.h"

#include "Components/BoxComponent.h"
#include "Enemy/Security_Guard.h"
#include "Kismet/GameplayStatics.h"
#include "Minigame/MiniGamePawn.h"
#include "EngineUtils.h"

AInteractebleMinigameTrigger::AInteractebleMinigameTrigger()
{
	BoxCollider = CreateDefaultSubobject<UBoxComponent>("BoxCollider");
	BoxCollider->SetupAttachment(GetRootComponent());

	MiniGameHaveBeenPossessed = false;
}

void AInteractebleMinigameTrigger::BeginPlay()
{
	Super::BeginPlay();

	CastToSecurityGuard();
	Tags.Add(FName("MiniGameTrigger"));
}

void AInteractebleMinigameTrigger::Interacted()
{
	AInteractable::Interacted();

	if (!MinigameToPosses)
	{
		//GEngine->AddOnScreenDebugMessage(-1,5,FColor::Red,TEXT("No Minigame selected"));	
		return;
	}
	PlayerController = UGameplayStatics::GetPlayerController(GetWorld(),0);
	if (!PlayerController)
	{
		return;
	}

	SecurityGuard->FreezeWhileMinigame();
	PlayerController->Possess(MinigameToPosses);
	MiniGameHaveBeenPossessed = true;
}

void AInteractebleMinigameTrigger::InteractedEnd()
{
	if (MiniGameHaveBeenPossessed == true)
	{
		SecurityGuard->UnFreezeAfterMinigame();
		SetActorEnableCollision(false);
	}
}

void AInteractebleMinigameTrigger::CastToSecurityGuard()
{
	FindAllActors(GetWorld(), AllSecurityGuards);
	if (AllSecurityGuards.IsEmpty())
	{
		//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, TEXT("No player character"));
		UE_LOG(LogTemp, Warning, TEXT("No Security Guard Found"))
		return;
	}

	if (AllSecurityGuards.Num() > 1)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, TEXT("More then 1 player character"));
		UE_LOG(LogTemp, Warning, TEXT("More then 1 Secuirty Guard"))
		return;
	}

	for (int i{}; i < AllSecurityGuards.Num(); i++)
	{
		SecurityGuard = Cast<ASecurity_Guard>(AllSecurityGuards[i]);
	}
}
