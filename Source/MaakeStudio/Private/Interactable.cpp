// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable.h"
#include "PlayerCharacter.h"
#include "components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"

// Sets default values
AInteractable::AInteractable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	

	SphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollider"));
	SetRootComponent(SphereCollider);

	InteractableMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	InteractableMesh->SetupAttachment(GetRootComponent());
	Tags.Add(FName("Interacteble"));
}

// Called when the game starts or when spawned
void AInteractable::BeginPlay()
{
	Super::BeginPlay();
	FindAllActors(GetWorld(), AllPlayers);
}

// Called every frame
void AInteractable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AInteractable::Interacted()
{
	GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, TEXT("I AM THE MASTER"));

	//Do Stuff

	

}

void AInteractable::Bolle()
{
	GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, TEXT("Bolle"));
}

void AInteractable::SoftReset()
{
}

void AInteractable::CastToPlayer()
{
	
	
	if(AllPlayers.IsEmpty())
	{
		GEngine->AddOnScreenDebugMessage(-1,10,FColor::Red,TEXT("No player character"));
		UE_LOG(LogTemp,Warning,TEXT("No player character"))
		return;
	}

	if(AllPlayers.Num() > 1)
	{
		GEngine->AddOnScreenDebugMessage(-1,10,FColor::Red,TEXT("More then 1 player character"));
		UE_LOG(LogTemp,Warning,TEXT("More then 1 player character"))
		return;
	}
	
	
	for (int i{}; i < AllPlayers.Num(); i++)
	{
		Player = Cast<APlayerCharacter>(AllPlayers[i]);

	
	
		
	}
	
	
}

