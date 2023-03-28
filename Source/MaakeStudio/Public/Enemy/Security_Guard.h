// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Security_Guard.generated.h"

class AAIController;

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

	AAIController* EnemyController;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	AActor* PatrolTarget;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "AI Navigation")
	double PatrolRadius = 200.f;

private:
	bool InTargetRange(AActor* Target, double Radius);
};
