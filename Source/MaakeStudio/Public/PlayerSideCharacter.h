// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerSideCharacter.generated.h"

class AAIController;

UCLASS()
class MAAKESTUDIO_API APlayerSideCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerSideCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USphereComponent* SphereCollider;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void WalkToPoint(FVector inWalkPoint);

	AAIController* PlayerSideController;

	UFUNCTION(CallInEditor, BlueprintCallable)
	void SoftReset();

	UFUNCTION(CallInEditor, BlueprintCallable)
	void Interact();

	//collision begin overlap
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	//collision end overlap
	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditAnywhere)
	TArray<class AInteractable*> OverlappingActors;

	//Audio Files
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Audio")
	USoundBase* SoundGrab;


	//Animations
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Animation")
	UAnimMontage* GrabAnimation;
	
private:
	FVector SpawnLocation;
	FRotator SpawnRotation;

	bool HasInteractableInRange;
	bool HasInteractableInRangeLastFrame;
	
	void InteractInRange();

	template<typename T>
	void FindAllActors(UWorld* World, TArray<T*>& Out)
	{
		for (TActorIterator<T> It(World); It; ++It)
		{
			Out.Add(*It);
		}
	}

	TArray<class APlayerCharacter*> PlayerCharacterArr;
};
