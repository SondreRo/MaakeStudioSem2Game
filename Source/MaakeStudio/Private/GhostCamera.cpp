// Fill out your copyright notice in the Description page of Project Settings.


#include "GhostCamera.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AGhostCamera::AGhostCamera()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));

	
	
}

// Called when the game starts or when spawned
void AGhostCamera::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGhostCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

