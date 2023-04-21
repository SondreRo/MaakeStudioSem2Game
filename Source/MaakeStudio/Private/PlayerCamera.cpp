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

	Indicator = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Indicator"));
	Indicator->SetupAttachment(GetRootComponent());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	
	Camera->SetRelativeLocation(FVector(0, 0, 3));
	Camera->SetRelativeRotation(FRotator(90, 0, 0));
	Camera->bUsePawnControlRotation = false;
	Camera->SetupAttachment(GetRootComponent());

	CameraPitch = 0;
	CameraYaw = 0;
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

	CameraPitch = FMath::Clamp(CameraPitch, -90.0f, 90.0f);
	CameraYaw = FMath::Clamp(CameraYaw, -90.0f, 90.0f);


	Camera->SetRelativeRotation(FRotator(CameraPitch, CameraYaw, 0));

}

void APlayerCamera::CamSelected()
{
	//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, TEXT("Camselected"));
	Indicator->SetHiddenInGame(false);
	

	//StaticMesh->SetMaterialByName()
}

void APlayerCamera::CamDeselected()
{
	Indicator->SetHiddenInGame(true);
	//StaticMesh->SetMaterial(0, NormalMaterial);
}

void APlayerCamera::AddCameraPitch(float input)
{
	CameraPitch += input;
}

void APlayerCamera::AddCameraYaw(float input)
{
	CameraYaw += input;
}

