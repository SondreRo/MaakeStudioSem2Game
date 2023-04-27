// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NPC.generated.h"

UENUM(BlueprintType)
enum class ENPCState : uint8
{
	Walking UMETA(DisplayName = "Walking"),
	Waiting UMETA(DisplayName = "Waiting"),
	DeSpawned UMETA(DisplayName = "DeSpawned")
};

class AAIController;

UCLASS()
class MAAKESTUDIO_API ANPC : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANPC();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//Public Methods
	UFUNCTION(BlueprintCallable)
	void DeSpawnNPC();
	UFUNCTION(BlueprintCallable)
	void SoftReset();

	//Public Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings")
	bool PlayTest = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings")
	float TotalWaitTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings")
	float WalkSpeed;

	UPROPERTY(EditInstanceOnly, Category = "AI Settings")
	TArray<AActor*> WalkTargets;
	AAIController* NPCController;

private:
	//Private Methods

	//Movement Methods
	void MoveToRandomPoint();
	void MoveTo(AActor* Target);
	bool InTargetRange(AActor* Target, double Radius);

	//Timer Methods
	void WaitTimer(float DeltaTime);
	void CheckStandingStill(float DeltaTime);

	//Private Variables
	AActor* WalkTarget;

	ENPCState NPCState;
	FVector CurrentPos;
	float WaitTime;
	float StandingStillTime;
	float TotalStillTime;
	double WalkRadius;
};
