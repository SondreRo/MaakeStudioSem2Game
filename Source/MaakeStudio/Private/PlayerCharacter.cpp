// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include <string>

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputTriggers.h"
#include "PlayerCamera.h"

#include "Engine/World.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	//SpringArm->SetupAttachment(GetRootComponent());
	//SpringArm->TargetArmLength = 0.f;
	//SpringArm->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(GetRootComponent());
	Camera->SetRelativeLocation(FVector(0, 0, 50));
	Camera->bUsePawnControlRotation = true;

	//Camera->AddLocalOffset(FVector(0, 0, 50));
	//Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);



	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	InputX = 0;
	InputY = 0;

	LineTraceLocation = FVector::ZeroVector;
	LineTraceNormal = FRotator::ZeroRotator;

	RayLength = 1000.f;
	SpawnedGhostCamera = nullptr;
	LineTraceHitSomething = false;
	SpawnedPlayerCameraArray = {};
	CameraMinDistance = 50;
	ToolSelected = 1;

	HoldingInteractButton = false;

	MaxCameras = 10;
	WalkSpeed = 400;
	RunSpeed = 600;
	IsRunning = false;

	SelectedCamera = nullptr;


}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = 400.f;
	GetCharacterMovement()->AirControl = 0.5f;

	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	if (PlayerController)
	{
		UEnhancedInputLocalPlayerSubsystem* subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (subsystem)
		{
			subsystem->AddMappingContext(InputMapping, 0);

		}
	}
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CharMovement();

	AddControllerYawInput(Yaw);
	AddControllerPitchInput(Pitch);
	float velocity = GetVelocity().Length();
	FString TheFloatStr = FString::SanitizeFloat(velocity);

	GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Red, TheFloatStr);
}

void APlayerCharacter::CharMovement()
{
	FRotator ControlRotation = Controller->GetControlRotation();
	ControlRotation.Roll = 0.f;
	ControlRotation.Pitch = 0.f;

	FVector ForwardVector = UKismetMathLibrary::GetForwardVector(ControlRotation);
	FVector RightVector = UKismetMathLibrary::GetRightVector(ControlRotation);

	ForwardVector *= InputX;
	RightVector *= InputY;

	if (!FMath::IsNearlyZero(InputX))
	{
		AddMovementInput(ForwardVector);
	}
	if (!FMath::IsNearlyZero(InputY))
	{
		AddMovementInput(RightVector);
	}
}

void APlayerCharacter::DestroyGhostCam()
{
	if (SpawnedGhostCamera != nullptr)
	{
		SpawnedGhostCamera->Destroy();
		SpawnedGhostCamera = nullptr;
	}
}

void APlayerCharacter::PlaceGhostCamera()
{
	FHitResult Hit;

	FVector TraceStart = Camera->GetComponentLocation();
	FVector TraceEnd = TraceStart + (Camera->GetForwardVector() * RayLength);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, TraceChannelProperty, QueryParams);

	//DrawDebugLine(GetWorld(), TraceStart, TraceEnd, Hit.bBlockingHit ? FColor::Blue : FColor::Red, false, 5.0f, 0, 10.0f);
	//DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 4.f, 4, FColor::Red, false, 0.3f, 0, 1.f);

	//UE_LOG(LogTemp, Log, TEXT("Tracing line: %s to %s"), *TraceStart.ToCompactString(), *TraceEnd.ToCompactString());


	UWorld* World = GetWorld();

	if (Hit.bBlockingHit && IsValid(Hit.GetActor()))
	{
		LineTraceHitSomething = true;
		if (World && !SpawnedGhostCamera)
		{
			SpawnedGhostCamera = World->SpawnActor<AActor>(GhostCamera, Hit.ImpactPoint, Hit.ImpactNormal.ToOrientationRotator());

		}

		if (SpawnedGhostCamera != nullptr)
		{
			FVector OldLocaton = SpawnedGhostCamera->GetActorLocation();
			FVector NewLocation = Hit.ImpactPoint;

			FRotator OldRotation = SpawnedGhostCamera->GetActorRotation();
			FRotator NewRotation = Hit.ImpactNormal.Rotation() + FRotator(-90, 0, 0);

			
			SpawnedGhostCamera->SetActorLocation(FMath::Lerp(OldLocaton, NewLocation, .3));
			//SpawnedGhostCamera->SetActorLocation(Hit.ImpactPoint);
			
			//SpawnedGhostCamera->SetActorRotation(FMath::Lerp(OldRotation, NewRotation, .5));
			SpawnedGhostCamera->SetActorRotation(NewRotation);

			//UE_LOG(LogTemp, Log, TEXT("CameraTransform: Location: %s Rotation: %s"), *NewLocation.ToCompactString(), *NewRotation.ToCompactString());
		}


		//UE_LOG(LogTemp, Log, TEXT("Trace hit actor: %s"), *Hit.GetActor()->GetName());
		LineTraceLocation = Hit.ImpactPoint;
		LineTraceNormal = Hit.ImpactNormal.Rotation();
	}
	else {
		LineTraceHitSomething = false;
		//UE_LOG(LogTemp, Log, TEXT("No Actors were hit"));



		DestroyGhostCam();
	}
}

void APlayerCharacter::CameraPlaceMode()
{
	if (SpawnedPlayerCameraArray.Num() >= MaxCameras)
	{
		return;
	}


	UWorld* World = GetWorld();
	//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, TEXT("End"));
	//DrawDebugSphere(GetWorld(), LineTraceLocation, 10.f, 4, FColor::Green, false, 1, 0, 1.f);
	if (CheckCameraPlacement(LineTraceLocation) && LineTraceHitSomething)
	{
		SpawnedPlayerCamera = World->SpawnActor<AActor>(PlayerCamera, LineTraceLocation, LineTraceNormal + FRotator(-90, 0, 0));
		SpawnedPlayerCameraArray.Add(SpawnedPlayerCamera);
	}
	DestroyGhostCam();
}

void APlayerCharacter::SelectMode()
{
	if (SelectedCamera != nullptr)
	{
		Cast<APlayerCamera>(SelectedCamera)->CamDeselected();
		
	}

	FHitResult Hit;

	FVector TraceStart = Camera->GetComponentLocation();
	FVector TraceEnd = Camera->GetComponentLocation() + Camera->GetForwardVector() * RayLength;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, SelectTraceChannelProperty, QueryParams);

	/*DrawDebugLine(GetWorld(), TraceStart, TraceEnd, Hit.bBlockingHit ? FColor::Blue : FColor::Red, false, 5.0f, 0, 10.0f);
	UE_LOG(LogTemp, Log, TEXT("Tracing line: %s to %s"), *TraceStart.ToCompactString(), *TraceEnd.ToCompactString());

	if (Hit.bBlockingHit && IsValid(Hit.GetActor()))
	{
		UE_LOG(LogTemp, Log, TEXT("Trace hit actor: %s"), *Hit.GetActor()->GetName());
	}
	else {
		UE_LOG(LogTemp, Log, TEXT("No Actors were hit"));
	}*/

	bool HasHitCamera = false;
	//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Green, TEXT("Ray"));
	for (int i{}; i < SpawnedPlayerCameraArray.Num(); i++)
	{
		if (SpawnedPlayerCameraArray[i] == Hit.GetActor())
		{
			HasHitCamera = true;
		}
	}

	if (HasHitCamera)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Green, TEXT("HitCamera"));
		SelectedCamera = Hit.GetActor();
		Cast<APlayerCamera>(SelectedCamera)->CamSelected();
	}

	

}

bool APlayerCharacter::CheckCameraPlacement(FVector HitLocation)
{
	if (SpawnedPlayerCameraArray.IsEmpty())
	{
		return true;
	}
	else
	{
		bool ReturnValue = true;

		for (int i{}; i < SpawnedPlayerCameraArray.Num(); i++)
		{
			float Distance = (HitLocation - SpawnedPlayerCameraArray[i]->GetActorLocation()).Length();
			if (Distance < CameraMinDistance)
			{
				ReturnValue = false;
			}
		}

		return ReturnValue;
	}
	
}


// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (UEnhancedInputComponent* EnhanceInputCom = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {

		EnhanceInputCom->BindAction(MovementForwardBackInput, ETriggerEvent::Triggered, this, &APlayerCharacter::MovementForwardBack);
		EnhanceInputCom->BindAction(MovementForwardBackInput, ETriggerEvent::Completed, this, &APlayerCharacter::MovementForwardBack);


		EnhanceInputCom->BindAction(MovementRightLeftInput, ETriggerEvent::Triggered, this, &APlayerCharacter::MovementRightLeft);
		EnhanceInputCom->BindAction(MovementRightLeftInput, ETriggerEvent::Completed, this, &APlayerCharacter::MovementRightLeft);


		EnhanceInputCom->BindAction(CameraPitchInput, ETriggerEvent::Triggered, this, &APlayerCharacter::CameraPitch);
		EnhanceInputCom->BindAction(CameraPitchInput, ETriggerEvent::Completed, this, &APlayerCharacter::CameraPitch);

		EnhanceInputCom->BindAction(CameraYawInput, ETriggerEvent::Triggered, this, &APlayerCharacter::CameraYaw);
		EnhanceInputCom->BindAction(CameraYawInput, ETriggerEvent::Completed, this, &APlayerCharacter::CameraYaw);

		EnhanceInputCom->BindAction(JumpInput, ETriggerEvent::Started, this, &APlayerCharacter::JumpTrigger);

		EnhanceInputCom->BindAction(RunInput, ETriggerEvent::Started, this, &APlayerCharacter::RunStart);
		EnhanceInputCom->BindAction(RunInput, ETriggerEvent::Completed, this, &APlayerCharacter::RunEnd);

		EnhanceInputCom->BindAction(MainActionInput, ETriggerEvent::Started, this, &APlayerCharacter::MainInteractStarted);
		EnhanceInputCom->BindAction(MainActionInput, ETriggerEvent::Triggered, this, &APlayerCharacter::MainInteractTrigger);
		EnhanceInputCom->BindAction(MainActionInput, ETriggerEvent::Completed, this, &APlayerCharacter::MainInteractEnd);

		//Keyboard Number Buttons
		EnhanceInputCom->BindAction(SwapToolOneInput, ETriggerEvent::Started, this, &APlayerCharacter::SwapToolOne);
		EnhanceInputCom->BindAction(SwapToolTwoInput, ETriggerEvent::Started, this, &APlayerCharacter::SwapToolTwo);


	}
}


void APlayerCharacter::MovementForwardBack(const FInputActionValue& input)
{
	InputX = input.Get<float>();
}

void APlayerCharacter::MovementRightLeft(const FInputActionValue& input)
{
	InputY = input.Get<float>();
}

void APlayerCharacter::CameraPitch(const FInputActionValue& input)
{
	Pitch = input.Get<float>();
}

void APlayerCharacter::CameraYaw(const FInputActionValue& input)
{
	Yaw = input.Get<float>();
}

void APlayerCharacter::JumpTrigger(const FInputActionValue& input)
{
	Jump();
}

void APlayerCharacter::DeleteTrigger(const FInputActionValue& input)
{
	if (SelectedCamera != nullptr)
	{
		SelectedCamera->Destroy();
	}
}

void APlayerCharacter::RunStart(const FInputActionValue& input)
{
	IsRunning = true;
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
}

void APlayerCharacter::RunEnd(const FInputActionValue& input)
{
	IsRunning = false;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void APlayerCharacter::MainInteractStarted(const FInputActionValue& input)
{
	if (!HoldingInteractButton)
	{
		HoldingInteractButton = true;
	}
	//Update Blueprint


	switch (ToolSelected)
	{
	case 1:
		SelectMode();

		break;
	//case 2:
		//Camera Mode
		//PlaceGhostCamera();
		//break;

	//default:
		//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, TEXT("Hello"));
	}
}

void APlayerCharacter::MainInteractTrigger(const FInputActionValue& input)
{
	//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Green, TEXT("Trigger"));

	if (!HoldingInteractButton)
	{
		HoldingInteractButton = true;
	}
	//Update Blueprint
	

	switch(ToolSelected)
	{
	case 1:
		//SelectMode();

		break;
	case 2:
		//Camera Mode
		PlaceGhostCamera();
		break;

	default:
		GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, TEXT("Hello"));
	}
	

	
	
}

void APlayerCharacter::MainInteractEnd(const FInputActionValue& input)
{
	DestroyGhostCam();
	if (HoldingInteractButton)
	{
		HoldingInteractButton = false;
	}

	switch (ToolSelected)
	{
	case 1:
		//Interact

		break;
	case 2:
		//Camera Mode
		CameraPlaceMode();
		break;

	default:
		GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, TEXT("Hello"));
	}
	
}

void APlayerCharacter::SwapToolOne(const FInputActionValue& input)
{
	
	if (HoldingInteractButton)
	{
		return;
	}

	ToolSelected = 1;
	UpdateHand(ToolSelected);
}

void APlayerCharacter::SwapToolTwo(const FInputActionValue& input)
{
	
	if (HoldingInteractButton)
	{
		return;
	}

	ToolSelected = 2;
	UpdateHand(ToolSelected);
}


