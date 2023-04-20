// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyCamera.h"

#include "Enemy/Security_Guard.h"
#include "Perception/PawnSensingComponent.h"
#include "EngineUtils.h"
#include "AIController.h"

// Sets default values
AEnemyCamera::AEnemyCamera()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TargetSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("TargetSensing"));
	TargetSensing->SightRadius = 2000.f;
	TargetSensing->SetPeripheralVisionAngle(45.f);
	TargetSensing->bOnlySensePlayers = false;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
}

// Called when the game starts or when spawned
void AEnemyCamera::BeginPlay()
{
	Super::BeginPlay();

	CastSecurityGuard();

	if (TargetSensing != nullptr)
	{
		TargetSensing->OnSeePawn.AddDynamic(this, &AEnemyCamera::TargetSeen);
	}
}

// Called every frame
void AEnemyCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemyCamera::TargetSeen(APawn* Target)
{
	if (Target->ActorHasTag("PlayerSideCharacter"))
	{
		TargetSeenLocation = Target->GetActorLocation();
		SecurityGuard->SendChasingTarget(TargetSeenLocation);
	}
	if (Target->ActorHasTag("PlayerCharacter"))
	{
		TargetSeenLocation = Target->GetActorLocation();
		SecurityGuard->SendChasingTarget(TargetSeenLocation);
	}
}

void AEnemyCamera::CastSecurityGuard()
{
	FindAllActors(GetWorld(), AllSecurityGuards);

	if (AllSecurityGuards.Num() > 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("More then 1 Security Guard"))
		return;
	}

	for (int i{}; i < AllSecurityGuards.Num(); i++)
	{
		SecurityGuard = Cast<ASecurity_Guard>(AllSecurityGuards[i]);
	}
}
