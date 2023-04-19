// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyCamera.generated.h"

class UPawnSensingComponent;
class ASecurity_Guard;
class AAIController;

UCLASS()
class MAAKESTUDIO_API AEnemyCamera : public APawn
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemyCamera();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void TargetSeen(APawn* Target);

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* StaticMesh;
	AAIController* EnemyCameraController;

	FVector TargetSeenLocation;
private:
	void CastSecurityGuard();

	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* TargetSensing;
};
