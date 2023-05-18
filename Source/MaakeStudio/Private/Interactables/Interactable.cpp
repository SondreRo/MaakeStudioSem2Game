// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/Interactable.h"

#include "PlayerCharacter.h"
#include "PlayerSideCharacter.h"
#include "EngineUtils.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AInteractable::AInteractable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	InteractableMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	SetRootComponent(InteractableMesh);
	//InteractableMesh->SetupAttachment(GetRootComponent());
	
	InteractableMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Tags.Add(FName("Interacteble"));
}

// Called when the game starts or when spawned
void AInteractable::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AInteractable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AInteractable::Interacted()
{
	//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, TEXT("I AM THE MASTER"));
	InteractBlueprint();
	AActor* TempActor;
	APlayerSideCharacter* PlayerSideCharacter;

	TempActor = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerSideCharacter::StaticClass());
	PlayerSideCharacter = Cast<APlayerSideCharacter>(TempActor);
	if (!PlayerSideCharacter) 
	{
		return;
	}
	float tempMaxWalkSpeed = PlayerSideCharacter->GetCharacterMovement()->MaxWalkSpeed;
	PlayerSideCharacter->GetCharacterMovement()->MaxWalkSpeed = (tempMaxWalkSpeed * 1.01);
}

void AInteractable::InteractedEnd()
{
	
}

void AInteractable::SoftReset()
{
	SoftResetBlueprint();
}

void AInteractable::CastToPlayer()
{
	FindAllActors(GetWorld(), AllPlayers);
	if (AllPlayers.IsEmpty())
	{
		//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, TEXT("No player character"));
		UE_LOG(LogTemp, Warning, TEXT("No player character"))
			return;
	}

	if (AllPlayers.Num() > 1)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, TEXT("More then 1 player character"));
		UE_LOG(LogTemp, Warning, TEXT("More then 1 player character"))
			return;
	}

	for (int i{}; i < AllPlayers.Num(); i++)
	{
		Player = Cast<APlayerCharacter>(AllPlayers[i]);
	}
}
