// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerSideCharacter.h"
#include "components/SphereComponent.h"
#include "Interactables/Interactable.h"
#include "PlayerCharacter.h"
#include "EngineUtils.h"

#include "AIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimInstance.h"

// Sets default values
APlayerSideCharacter::APlayerSideCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollider"));
	SphereCollider->SetupAttachment(GetRootComponent());

	//OverlappingActors.Init(nullptr, 10);
	HasInteractableInRange = false;
	HasInteractableInRangeLastFrame = false;
	
}

// Called when the game starts or when spawned
void APlayerSideCharacter::BeginPlay()
{
	Super::BeginPlay();
	PlayerSideController = Cast<AAIController>(GetController());

	Tags.Add(FName("PlayerSideCharacter"));

	SpawnLocation = GetActorLocation();
	SpawnRotation = GetActorRotation();

	SphereCollider->OnComponentBeginOverlap.AddDynamic(this, &APlayerSideCharacter::OnOverlapBegin);
	SphereCollider->OnComponentEndOverlap.AddDynamic(this, &APlayerSideCharacter::OnOverlapEnd);

	GetCharacterMovement()->bOrientRotationToMovement = true;

}

// Called every frame
void APlayerSideCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	HasInteractableInRange = !OverlappingActors.IsEmpty();

	if (HasInteractableInRange != HasInteractableInRangeLastFrame)
	{
		InteractInRange();
	}
	
	HasInteractableInRangeLastFrame = HasInteractableInRange;
	
}

// Called to bind functionality to input
void APlayerSideCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void APlayerSideCharacter::WalkToPoint(FVector inWalkPoint)
{
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalLocation(inWalkPoint);
	MoveRequest.SetAcceptanceRadius(15.f);
	//MoveRequest.SetCanStrafe(false);


	FNavPathSharedPtr NavPath;

	PlayerSideController->MoveTo(MoveRequest, &NavPath);

}

void APlayerSideCharacter::SoftReset()
{
	SetActorLocation(SpawnLocation);
	SetActorRotation(SpawnRotation);
	WalkToPoint(SpawnLocation);
}

void APlayerSideCharacter::Interact()
{
	//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Blue, TEXT("Hello from Side Character"));
	//Jump();

	if (OverlappingActors.IsEmpty())
	{
		return;
	}

	int OverlappingIndex = -1;
	float ShortestVector = 1000000;
	
	for (int i = OverlappingActors.Num()-1; i >= 0; i--)
	{
		FString ActorName = OverlappingActors[i]->GetName();
		//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, ActorName);

		float VectorLength = FMath::Abs((OverlappingActors[i]->GetActorLocation() - GetActorLocation()).Size());
					
		if (VectorLength < ShortestVector)
		{
			ShortestVector = VectorLength;
			OverlappingIndex = i;
		}
	}
	
	if (OverlappingIndex < 0)
	{
		return;
	}
	
	OverlappingActors[OverlappingIndex]->Interacted();
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), SoundGrab,GetActorLocation());
	//GetMesh()->PlayAnimation(GrabAnimation, false);
	//UAnimInstance::Montage_Play(GrabAnimation,1,EMontagePlayReturnType::Duration,0,true);
	PlayAnimMontage(GrabAnimation);
}

void APlayerSideCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AInteractable* InteractableActor = Cast<AInteractable>(OtherActor);

	if (InteractableActor == nullptr)
	{
				return;
	}
	if (OverlappingActors.Contains(InteractableActor))
	{
				return;
	}

	OverlappingActors.Add(InteractableActor);
	//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, TEXT("IHIT SOMETHING"));
}

void APlayerSideCharacter::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AInteractable* InteractableActor = Cast<AInteractable>(OtherActor);

	if (InteractableActor == nullptr)
	{
		return;
	}

	if (OverlappingActors.Contains(InteractableActor) )
	{
		OverlappingActors.RemoveSwap(InteractableActor, true);
	}
	if (InteractableActor->ActorHasTag("MiniGameTrigger"))
	{
		InteractableActor->InteractedEnd();
	}
}

void APlayerSideCharacter::InteractInRange()
{
	FindAllActors(GetWorld(), PlayerCharacterArr);
	
	//GEngine->AddOnScreenDebugMessage(-1,5,FColor::Red,TEXT("CanInteract"));
	//GEngine->AddOnScreenDebugMessage(-1,5,FColor::Red,FString::FromInt(!OverlappingActors.IsEmpty()));
	APlayerCharacter* PlayerCharacter = nullptr;
	// if(PlayerCharacterArr.IsEmpty())
	// {
	// 	return;
	// }
	
	for (int i{}; i < PlayerCharacterArr.Num(); i++)
	{
		PlayerCharacter = Cast<APlayerCharacter>(PlayerCharacterArr[i]);
	}

	if (PlayerCharacter == nullptr)
	{
		return;
	}
	//GEngine->AddOnScreenDebugMessage(-1,5,FColor::Red,TEXT("I have aids"));
	PlayerCharacter->CanInteract = !OverlappingActors.IsEmpty();
}