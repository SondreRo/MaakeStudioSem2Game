// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Security_Guard.generated.h"

class AAIController;
class UPawnSensingComponent;
class APlayerCharacter;

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	Patrolling UMETA(DisplayName = "Patrolling"),
	Checking UMETA(DisplayName = "Checking"),
	Chasing UMETA(DisplayName = "Chasing"),
	Waiting UMETA(DisplayName = "Waiting"),
	Frozen UMETA(DisplayName = "Frozen")
};

UCLASS()
class MAAKESTUDIO_API ASecurity_Guard : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASecurity_Guard();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	//Public Methods
	UFUNCTION()
	void TargetSeen(APawn* Target);
	UFUNCTION(CallInEditor, BlueprintCallable)
	void SoftReset();

	void SendChasingTarget(FVector& location);
	void FreezeWhileMinigame();
	void UnFreezeAfterMinigame();

	//Public Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings")
	bool PlayTest = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings")
	bool RandomPatrolling = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings")
	float TotalWaitTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings")
	float TotalAggroTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings")
	float WalkSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings")
	float ChaseSpeed;

	UPROPERTY(EditInstanceOnly, Category = "AI Settings")
	TArray<AActor*> PatrolTargets;
	AAIController* EnemyController;

private:
	//Private Methods
	void AI_TickRun(float DeltaTime);

	//Movement Methods
	void MoveToPoint();
	void MoveToRandomPoint();
	void MoveToLocation(FVector& location);
	void MoveTo(AActor* Target);
	void ChasingTarget(APawn* Target);

	//Other 
	bool InTargetRange(AActor* Target, double Radius);
	bool InTargetRange(FVector& location, double Radius);
	void PauseWhenFinished(float DeltaTime);
	void AggroTimer(float DeltaTime);
	void AddPatrolTargets();
	void CheckingLocation();

	//Private Variables
	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* TargetSensing;
	TArray<AActor*> TestTargets;
	AActor* PatrolTarget;
	AActor* ChaseTarget;
	APlayerCharacter* PlayerCharacter;

	EEnemyState EnemyState;
	EEnemyState TempEnemyState;
	FVector SpawnLocation;
	FVector CheckLocation;
	FVector TempLocation;
	int PatrolTargetNumber;
	double PatrolRadius;
	double ChaseRadius;
	double CheckRadius;
	bool Waiting;
	float WaitTimer;
	float AggroTime;
	bool CatchedPlayer;
	float TempSpeed;
};