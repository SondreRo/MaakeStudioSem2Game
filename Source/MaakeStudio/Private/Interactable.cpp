// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable.h"
#include "components/SphereComponent.h"

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
	
}

// Called every frame
void AInteractable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AInteractable::Interacted()
{
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Hello From Interacteble"));

	//Do Stuff


}

