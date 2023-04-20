// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"


#include "CachedGeometry.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputTriggers.h"
#include "PlayerCamera.h"
#include "PlayerSideCharacter.h"
#include "EngineUtils.h"

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
	Camera->SetRelativeLocation(FVector(0, 0, 80));
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
	CameraMinDistance = 100;
	ToolSelected = 1;

	HoldingInteractButton = false;

	MaxCameras = 10;
	WalkSpeed = 400;
	RunSpeed = 600;
	IsRunning = false;

	SelectedCamera = nullptr;
	CurrentActiveCamera = nullptr;


	CameraViewMode = false;	
	CameraToChangeTo = 0;

	SideCharacterRayLength = 10000.f;

	SeenPlacingCamera = false;
	TotalSusTime = 2;
	SusTimer = 0;
	CanInteract = true;
	isPossesed = true;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	//Temp
	
	
	GetCharacterMovement()->MaxWalkSpeed = 400.f;
	GetCharacterMovement()->AirControl = 0.5f;

	PlayerController = Cast<APlayerController>(Controller);
	if (PlayerController)
	{
		UEnhancedInputLocalPlayerSubsystem* subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (subsystem)
		{
			subsystem->AddMappingContext(InputMapping, 0);

		}
	}

	
	FindAllActors(GetWorld(), AllActorsToControll);

	if (AllActorsToControll.Num() == 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, TEXT("No Side Character Found"));
	}
	
	SpawnLocation = GetActorLocation();
	SpawnRotation = GetActorRotation();

	Tags.Add(FName("PlayerCharacter"));
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!PlayerController)
	{
		return;
	}
	if (!isPossesed)
	{
		return;
	}
	CharMovement();

	AddControllerYawInput(Yaw);
	AddControllerPitchInput(Pitch);

	SeenPlacingCameraTimer(DeltaTime);

	/*float velocity = GetVelocity().Length();
	FString TheFloatStr = FString::SanitizeFloat(velocity);

	GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Red, TheFloatStr);*/
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
	//GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, SelectTraceChannelProperty, QueryParams);
	GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility, QueryParams);
	
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
		GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, TEXT("HitCamera"));
		SelectedCamera = Hit.GetActor();
		Cast<APlayerCamera>(SelectedCamera)->CamSelected();
	}
	else
	{
		SelectedCamera = nullptr;
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

void APlayerCharacter::DestroyGhostCam()
{
	if (SpawnedGhostCamera != nullptr)
	{
		SpawnedGhostCamera->Destroy();
		SpawnedGhostCamera = nullptr;
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
		SpawnedPlayerCamera = World->SpawnActor<AActor>(PlayerCamera, LineTraceLocation, LineTraceNormal);
		SpawnedPlayerCameraArray.Add(SpawnedPlayerCamera);

		Tags.Add(FName("Sus"));
		SeenPlacingCamera = true;
	}

	DestroyGhostCam();
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

		EnhanceInputCom->BindAction(MovementRightLeftInput, ETriggerEvent::Started, this, &APlayerCharacter::MovementRightLeftStarted);


		EnhanceInputCom->BindAction(MovementRightLeftInput, ETriggerEvent::Triggered, this, &APlayerCharacter::MovementRightLeft);
		EnhanceInputCom->BindAction(MovementRightLeftInput, ETriggerEvent::Completed, this, &APlayerCharacter::MovementRightLeft);



		EnhanceInputCom->BindAction(CameraPitchInput, ETriggerEvent::Triggered, this, &APlayerCharacter::CameraPitch);
		EnhanceInputCom->BindAction(CameraPitchInput, ETriggerEvent::Completed, this, &APlayerCharacter::CameraPitch);

		EnhanceInputCom->BindAction(CameraYawInput, ETriggerEvent::Triggered, this, &APlayerCharacter::CameraYaw);
		EnhanceInputCom->BindAction(CameraYawInput, ETriggerEvent::Completed, this, &APlayerCharacter::CameraYaw);

		EnhanceInputCom->BindAction(JumpInput, ETriggerEvent::Started, this, &APlayerCharacter::JumpTrigger);

		EnhanceInputCom->BindAction(CameraModeToggleInput, ETriggerEvent::Started, this, &APlayerCharacter::CameraModeToggleTrigger);


		EnhanceInputCom->BindAction(RunInput, ETriggerEvent::Started, this, &APlayerCharacter::RunStart);
		EnhanceInputCom->BindAction(RunInput, ETriggerEvent::Completed, this, &APlayerCharacter::RunEnd);

		EnhanceInputCom->BindAction(MainActionInput, ETriggerEvent::Started, this, &APlayerCharacter::MainInteractStarted);
		EnhanceInputCom->BindAction(MainActionInput, ETriggerEvent::Triggered, this, &APlayerCharacter::MainInteractTrigger);
		EnhanceInputCom->BindAction(MainActionInput, ETriggerEvent::Completed, this, &APlayerCharacter::MainInteractEnd);


		EnhanceInputCom->BindAction(InteractInput, ETriggerEvent::Started, this, &APlayerCharacter::InteractStarted);
		EnhanceInputCom->BindAction(InteractInput, ETriggerEvent::Triggered, this, &APlayerCharacter::InteractTrigger);
		EnhanceInputCom->BindAction(InteractInput, ETriggerEvent::Completed, this, &APlayerCharacter::InteractEnd);

		//Delete
		EnhanceInputCom->BindAction(DeleteInput, ETriggerEvent::Triggered, this, &APlayerCharacter::DeleteTrigger);

		//Keyboard Number Buttons

		EnhanceInputCom->BindAction(ScrollWheelInput, ETriggerEvent::Triggered, this, &APlayerCharacter::ScrollWheelTrigger);

		EnhanceInputCom->BindAction(SwapToolOneInput, ETriggerEvent::Started, this, &APlayerCharacter::SwapToolOne);
		EnhanceInputCom->BindAction(SwapToolTwoInput, ETriggerEvent::Started, this, &APlayerCharacter::SwapToolTwo);


	}
}


void APlayerCharacter::SoftReset(bool DeleteCameras)
{
	SetActorLocation(SpawnLocation);
	SetActorRotation(SpawnRotation);

	GetCharacterMovement()->StopMovementImmediately();

	if (DeleteCameras)
	{
		DeleteAllCameras();
	}
		
	

}

void APlayerCharacter::AddGameScore(float inScore)
{
	GameScore += inScore;

	FString textToPrint = FString::SanitizeFloat(GameScore);
	GEngine->AddOnScreenDebugMessage(-1,3,FColor::Green,TEXT("TEST"));
	GEngine->AddOnScreenDebugMessage(-1,3,FColor::Green,textToPrint);
}

void APlayerCharacter::MovementForwardBack(const FInputActionValue& input)
{
	if (CameraViewMode)
	{
		InputX = 0;
		return;
	}

	InputX = input.Get<float>();
}

void APlayerCharacter::MovementRightLeft(const FInputActionValue& input)
{
	if (CameraViewMode)
	{
		InputY = 0;
		return;
	}
	InputY = input.Get<float>();
}

void APlayerCharacter::MovementRightLeftStarted(const FInputActionValue& input)
{
	if (CameraViewMode)
	{
		if (FMath::IsNearlyZero(input.Get<float>()))
		{
			return;
		}
		


		if (input.Get<float>() > 0)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Cyan, FString::FromInt(CameraToChangeTo));
			GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Cyan, TEXT("HELLO"));
			CameraToChangeTo++;
			if (CameraToChangeTo > SpawnedPlayerCameraArray.Num() - 1)
			{
				CameraToChangeTo = 0;
			}
			

		}
		else if (input.Get<float>() < 0)
		{
			CameraToChangeTo--;
			GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Cyan, FString::FromInt(CameraToChangeTo));

			if (CameraToChangeTo < 0)
			{
				CameraToChangeTo = SpawnedPlayerCameraArray.Num() - 1;
			}
		}
		ChangeViewTarget(CameraToChangeTo);
	}

	
}

void APlayerCharacter::DeleteAllCameras()
{
	if (SpawnedPlayerCameraArray.Num() > 0)
	{
		for (int i = 0; i < SpawnedPlayerCameraArray.Num(); i++)
		{
			SpawnedPlayerCameraArray[i]->Destroy();
		}
		SpawnedPlayerCameraArray.Empty();
	}
	
}

void APlayerCharacter::CameraPitch(const FInputActionValue& input)
{
	if (CameraViewMode)
	{
		if (CurrentActiveCamera)
		{
			Cast<APlayerCamera>(CurrentActiveCamera)->AddCameraPitch(-(input.Get<float>()));
		}


		Pitch = 0;
		return;
	}
	Pitch = input.Get<float>();
}

void APlayerCharacter::CameraYaw(const FInputActionValue& input)
{
	if (CameraViewMode)
	{

		if (CurrentActiveCamera)
		{
			Cast<APlayerCamera>(CurrentActiveCamera)->AddCameraYaw(input.Get<float>());
		}

		Yaw = 0;
		return;
	}
	Yaw = input.Get<float>();
}

void APlayerCharacter::JumpTrigger(const FInputActionValue& input)
{
	if (CameraViewMode)
	{
		return;
	}
	Jump();
}

void APlayerCharacter::DeleteTrigger(const FInputActionValue& input)
{
	if (SelectedCamera != nullptr)
	{
		AActor* CameraToDestroy = SelectedCamera;
		SpawnedPlayerCameraArray.RemoveSwap(SelectedCamera);
		
		CameraToDestroy->Destroy();
		CameraToDestroy = nullptr;

	}
}

void APlayerCharacter::CameraModeToggleTrigger(const FInputActionValue& input)
{
	if (CameraViewMode)
	{
		CameraViewMode = false;
		ChangeViewTarget(-1);
	}
	else
	{
		ChangeViewTarget(CameraToChangeTo);
		CameraViewMode = true;
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
	
		if (AllActorsToControll.Num() == 0)
		{
			return;
		}
			
		for (int i{}; i < AllActorsToControll.Num(); i++)
		{
			Cast<APlayerSideCharacter>(AllActorsToControll[i])->WalkToPoint(AllActorsToControll[i]->GetActorLocation());
		}


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

	if (CameraViewMode)
	{
		WalkSideCharacterToMouseCursor();
		return;
	}

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

	if (CameraViewMode)
	{
		ShootRayForSideCharacter();
		return;
	}
	Timer = 0;

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

void APlayerCharacter::InteractStarted(const FInputActionValue& input)
{
	
}

void APlayerCharacter::InteractTrigger(const FInputActionValue& input)
{
	if (!CanInteract)
	{
		return;
	}
	Timer++;
	if (Timer > 100)
	{
		if (AllActorsToControll.IsEmpty())
		{
			return;
		}

		for (int i{}; i < AllActorsToControll.Num(); i++)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Blue, TEXT("OneButtonPress"));
			AllActorsToControll[i]->Interact();
		}
		Timer = 0;
	}
}

void APlayerCharacter::InteractEnd(const FInputActionValue& input)
{
	Timer = 0;
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

void APlayerCharacter::ScrollWheelTrigger(const FInputActionValue& input)
{
	ScrollWheelValue = input.Get<float>();

	if  (FMath::IsNearlyZero(ScrollWheelValue))
	{
			return;
	}


	if (ScrollWheelValue > 0)
	{
		ToolSelected++;
		
	}
	else if (ScrollWheelValue < 0)
	{
		ToolSelected--;
		
	}


	if  (ToolSelected >= 3)
	{
		ToolSelected = 1;
	}
	if (ToolSelected <= 0)
	{
		ToolSelected = 2;
	}
		UpdateHand(ToolSelected);
}

void APlayerCharacter::ChangeViewTarget(int CameraIndex)
{
	if (PlayerController == nullptr)
	{
		return;
	}
	if (SpawnedPlayerCameraArray.IsEmpty())
	{
		GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Blue, TEXT("CantChangeCam"));

		return;

	}
	AActor* NewViewTarget = nullptr;
	if (CameraIndex <= -1)
	{
		NewViewTarget = this;

	}
	else if (CameraIndex < SpawnedPlayerCameraArray.Num())
	{
		NewViewTarget = SpawnedPlayerCameraArray[CameraIndex];
	}
	
	if (CameraIndex > SpawnedPlayerCameraArray.Num())
	{
		return;
	}

	if (NewViewTarget)
	{
		CurrentActiveCamera = NewViewTarget;
	}

	

	

	if (CameraIndex <= -1)
	{
		NewViewTarget = this;
	}
	GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Blue, TEXT("ChanginViewTarget"));

	PlayerController->SetViewTargetWithBlend(
		NewViewTarget,
		0.5f,
		EViewTargetBlendFunction::VTBlend_Cubic,
		1.f,
		true);


}

void APlayerCharacter::ShootRayForSideCharacter()
{
	if (!CameraViewMode)
	{
		return;
	}

	APlayerCamera* CurrentCamTest;

	if (Cast<APlayerCamera>(CurrentActiveCamera))
	{
		CurrentCamTest = Cast<APlayerCamera>(CurrentActiveCamera);
	}
	else
	{
		return;
	}

	

	FHitResult Hit;
	FVector TraceStart = CurrentCamTest->Camera->GetComponentLocation();
	FVector TraceEnd = TraceStart + (CurrentCamTest->Camera->GetForwardVector() * SideCharacterRayLength);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(CurrentCamTest);

	//GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, TraceChannelProperty, QueryParams);
	GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility, QueryParams);
	
	DrawDebugLine(GetWorld(), TraceStart, TraceEnd, Hit.bBlockingHit ? FColor::Blue : FColor::Red, false, 5.0f, 0, 3.0f);

	if (Hit.GetActor())
	{
		UE_LOG(LogTemp, Log, TEXT("Trace hit actor: %s"), *Hit.GetActor()->GetName());

	}


	if (CheckSideCharacterLineOfSight(CurrentCamTest))
	{
		if (Hit.ImpactPoint != FVector(0, 0, 0))
		{

			if (AllActorsToControll.Num() == 0)
			{
				return;
			}
			
			for (int i{}; i < AllActorsToControll.Num(); i++)
			{
				Cast<APlayerSideCharacter>(AllActorsToControll[i])->WalkToPoint(Hit.ImpactPoint);
			}
		}
	}
}

void APlayerCharacter::WalkSideCharacterToMouseCursor()
{
	if (!CameraViewMode)
	{
		return;
	}

	APlayerCamera* CurrentCamTest;

	if (Cast<APlayerCamera>(CurrentActiveCamera))
	{
		CurrentCamTest = Cast<APlayerCamera>(CurrentActiveCamera);
	}
	else
	{
		return;
	}

	

	FHitResult Hit;
	FVector TraceStart = CurrentCamTest->Camera->GetComponentLocation();
	FVector TraceEnd = TraceStart + (CurrentCamTest->Camera->GetForwardVector() * SideCharacterRayLength);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(CurrentCamTest);

	//GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, TraceChannelProperty, QueryParams);
	GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility, QueryParams);

	if (Hit.ImpactPoint != FVector(0, 0, 0))
	{

		if (AllActorsToControll.Num() == 0)
		{
			return;
		}
			
		for (int i{}; i < AllActorsToControll.Num(); i++)
		{
			FVector SideLocation = AllActorsToControll[i]->GetActorLocation();

			FVector MouseLocation = Hit.Location;

			FVector DirectionVector = MouseLocation - SideLocation;
			DirectionVector.Normalize();

			Cast<APlayerSideCharacter>(AllActorsToControll[i])->AddMovementInput(DirectionVector);
			
		}
			
			//Cast<APlayerSideCharacter>(AllActorsToControll[i])->WalkToPoint(Hit.ImpactPoint);
		
	}

	
}

bool APlayerCharacter::CheckSideCharacterLineOfSight(APlayerCamera* CurrentCam)
{
	//This is temp
	return true;
	
	if (AllActorsToControll.Num() == 0)
	{
		return false;
	}


	FHitResult Hit;
	FVector TraceStart = CurrentCam->Camera->GetComponentLocation();
	FVector TraceEnd = Cast<APlayerSideCharacter>(AllActorsToControll[0])->GetActorLocation();

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(CurrentCam);

	GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, TraceChannelProperty, QueryParams);

	DrawDebugLine(GetWorld(), TraceStart, TraceEnd, Hit.bBlockingHit ? FColor::Blue : FColor::Red, false, 5.0f, 0, 3.0f);


	if (Hit.GetActor() == AllActorsToControll[0])
	{
				return true;
	}

	return false;
}

void APlayerCharacter::SeenPlacingCameraTimer(float DeltaTime)
{
	if (SeenPlacingCamera == false)
	{
		return;
	}

	SusTimer += DeltaTime;

	if (SusTimer >= TotalSusTime)
	{
		SusTimer = 0;
		Tags.RemoveSingle(FName("Sus"));
		SeenPlacingCamera = false;
	}
}

void APlayerCharacter::UnPossessed()
{
	Super::UnPossessed();

	isPossesed = false;
}

void APlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	isPossesed = true;
	GEngine->AddOnScreenDebugMessage(-1,5,FColor::Red,TEXT("ISPossesed"));
	
	
	int ParameterToPass = CameraToChangeTo ; // You can use any supported variable type

	FTimerHandle TimerHandle;
	FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this, &APlayerCharacter::ChangeViewTarget, ParameterToPass);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 0.2, false);

	
}
