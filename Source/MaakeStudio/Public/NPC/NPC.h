// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NPC.generated.h"

UENUM(BlueprintType)
enum class ENPCState : uint8
{
	Walking UMETA(DisplayName = "Walking"),
	Waiting UMETA(DisplayName = "Waiting")
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

	AAIController* NPCController;
	UPROPERTY(EditInstanceOnly, Category = "AI Settings")
	TArray<AActor*> WalkTargets;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings")
	bool PlayTest = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings")
	float TotalWaitTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings")
	float WalkSpeed;

private:
	void MoveToRandomPoint();
	void MoveTo(AActor* Target);
	bool InTargetRange(AActor* Target, double Radius);
	void WaitTimer(float DeltaTime);
	void SoftReset();
	void CheckStandingStill(float DeltaTime);

	ENPCState NPCState;
	AActor* WalkTarget;
	FVector CurrentPos;

	float WaitTime;
	float StandingStillTime;
	float TotalStillTime;
	double WalkRadius;
};
