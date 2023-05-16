// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.generated.h"

UCLASS()
class MAAKESTUDIO_API AInteractable : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractable();

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	class UStaticMeshComponent* InteractableMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USphereComponent* SphereCollider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBoxComponent* BoxCollider;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	virtual void Interacted();

	UFUNCTION(BlueprintImplementableEvent, Category = "Calleble")
	void InteractBlueprint();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Calleble")
	void SoftResetBlueprint();
	
	UFUNCTION(BlueprintCallable)
	virtual void InteractedEnd();

	UFUNCTION(blueprintcallable)
	virtual void SoftReset();

	UFUNCTION(BlueprintCallable)
	virtual void CastToPlayer();

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool HasSphereComponent = true;
	
	template<typename T>
	void FindAllActors(UWorld* World, TArray<T*>& Out)
	{
		for (TActorIterator<T> It(World); It; ++It)
		{
			Out.Add(*It);
		}
	}

	TArray<class APlayerCharacter*> AllPlayers;
	APlayerCharacter* Player;

	//Public Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "GameScore")
	float GameScore;

	FVector SpawnLocation;
};