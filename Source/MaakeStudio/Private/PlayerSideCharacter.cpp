// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerSideCharacter.h"
#include "components/SphereComponent.h"
#include "Interactable.h"

#include "AIController.h"

// Sets default values
APlayerSideCharacter::APlayerSideCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollider"));
	SphereCollider->SetupAttachment(GetRootComponent());

	//OverlappingActors.Init(nullptr, 10);

	
}

// Called when the game starts or when spawned
void APlayerSideCharacter::BeginPlay()
{
	Super::BeginPlay();
	PlayerSideController = Cast<AAIController>(GetController());

	


	Tags.Add(FName("test"));

	SpawnLocation = GetActorLocation();
	SpawnRotation = GetActorRotation();

	
	SphereCollider->OnComponentBeginOverlap.AddDynamic(this, &APlayerSideCharacter::OnOverlapBegin);
	SphereCollider->OnComponentEndOverlap.AddDynamic(this, &APlayerSideCharacter::OnOverlapEnd);
}

// Called every frame
void APlayerSideCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
	GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Blue, TEXT("Hello from Side Character"));
	Jump();

	if (OverlappingActors.IsEmpty())
	{
		return;
	}

	for (int i{}; i < OverlappingActors.Num(); i++)
	{
		FString ActorName = OverlappingActors[i]->GetName();

		GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, ActorName);
		OverlappingActors[i]->Interacted();
	}
}

void APlayerSideCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//if (ActorHasTag("Interacteble"))
	//{
	//	OverlappingActors.Add(OtherActor);
	//}

	

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
	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, TEXT("IHIT SOMETHING"));
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
}



