// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCamera.h"

#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"

// Sets default values
APlayerCamera::APlayerCamera()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollider"));
	SetRootComponent(SphereCollider);

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(GetRootComponent());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	
	Camera->SetRelativeLocation(FVector(0, 0, 3));
	Camera->SetRelativeRotation(FRotator(90, 0, 0));
	Camera->bUsePawnControlRotation = false;
	Camera->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void APlayerCamera::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlayerCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APlayerCamera::CamSelected()
{
	StaticMesh->SetMaterial(0, SelectedMaterial);
	//StaticMesh->SetMaterialByName()
}

void APlayerCamera::CamDeselected()
{
	StaticMesh->SetMaterial(0, NormalMaterial);
}

