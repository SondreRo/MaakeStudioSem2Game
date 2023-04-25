// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactables/Interactable.h"
#include "InteractebleMinigameTrigger.generated.h"

class ASecurity_Guard;

UCLASS()
class MAAKESTUDIO_API AInteractebleMinigameTrigger : public AInteractable
{
	GENERATED_BODY()

	AInteractebleMinigameTrigger();

public:

protected:
	virtual void BeginPlay() override;
public:
	virtual void Interacted() override;
	virtual void InteractedEnd() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class AMiniGamePawn* MinigameToPosses;

	template<typename T>
	void FindAllActors(UWorld* World, TArray<T*>& Out)
	{
		for (TActorIterator<T> It(World); It; ++It)
		{
			Out.Add(*It);
		}
	}

	TArray<ASecurity_Guard*> AllSecurityGuards;
	ASecurity_Guard* SecurityGuard;

private:
	void CastToSecurityGuard();

	APlayerController* PlayerController;
};
