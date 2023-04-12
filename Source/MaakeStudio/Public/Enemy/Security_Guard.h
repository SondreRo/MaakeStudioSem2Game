// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Security_Guard.generated.h"

class AAIController;
class UPawnSensingComponent;

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	Patrolling UMETA(DisplayName = "Patrolling"),
	Chasing UMETA(DisplayName = "Chasing"),
	Waiting UMETA(DisplayName = "Waiting")
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
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//Public Functions
	UFUNCTION()
	void TargetSeen(APawn* Target);

	AAIController* EnemyController;
	UPROPERTY(EditInstanceOnly, Category = "AI Settings")
	TArray<AActor*> PatrolTargets;

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

private:
	//private Functions
	void MoveToPoint();
	void MoveToRandomPoint();
	bool InTargetRange(AActor* Target, double Radius);
	void MoveTo(AActor* Target);
	void PauseWhenFinished(float DeltaTime);
	void AggroTimer(float DeltaTime);
	void ChasingTarget(APawn* Target);

	void AddPatrolTargets();

	AActor* PatrolTarget;
	AActor* ChaseTarget;
	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* TargetSensing;
	EEnemyState EnemyState;

	TArray<AActor*> TestTargets;
	//private Variables
	int PatrolTargetNumber;
	double PatrolRadius;
	bool Waiting;
	float WaitTimer;
	float AggroTime;
};
