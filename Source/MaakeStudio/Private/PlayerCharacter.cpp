// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputTriggers.h"

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

	RayLength = 1000.f;
	SpawnedGhostCamera = nullptr;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = 400.f;

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

		EnhanceInputCom->BindAction(MainActionInput, ETriggerEvent::Triggered, this, &APlayerCharacter::MainInteractTrigger);
		EnhanceInputCom->BindAction(MainActionInput, ETriggerEvent::Completed, this, &APlayerCharacter::MainInteractEnd);

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

void APlayerCharacter::MainInteractTrigger(const FInputActionValue& input)
{
	//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Green, TEXT("Trigger"));

	FHitResult Hit;

	FVector TraceStart = Camera->GetComponentLocation();
	FVector TraceEnd = TraceStart + (Camera->GetForwardVector() * RayLength);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, TraceChannelProperty, QueryParams);

	//DrawDebugLine(GetWorld(), TraceStart, TraceEnd, Hit.bBlockingHit ? FColor::Blue : FColor::Red, false, 5.0f, 0, 10.0f);
	DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 10.f, 8, FColor::Red,false,1,0,1.f);

	UE_LOG(LogTemp, Log, TEXT("Tracing line: %s to %s"), *TraceStart.ToCompactString(), *TraceEnd.ToCompactString());


	UWorld* World = GetWorld();

	if (Hit.bBlockingHit && IsValid(Hit.GetActor()))
	{

		if (World && !SpawnedGhostCamera)
		{
			SpawnedGhostCamera = World->SpawnActor<AActor>(GhostCamera, Hit.ImpactPoint, Hit.ImpactNormal.ToOrientationRotator());

		}

		if(SpawnedGhostCamera != nullptr)
		{
			SpawnedGhostCamera->SetActorLocation(Hit.ImpactPoint);
			//SpawnedGhostCamera->SetActorRotation(Hit.ImpactNormal.ToOrientationRotator());
		}


		UE_LOG(LogTemp, Log, TEXT("Trace hit actor: %s"), *Hit.GetActor()->GetName());
		LineTraceLocation = Hit.ImpactPoint;
	}
	else {
		UE_LOG(LogTemp, Log, TEXT("No Actors were hit"));


		DestroyGhostCam();
	}
	
}

void APlayerCharacter::MainInteractEnd(const FInputActionValue& input)
{
	//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, TEXT("End"));
	DrawDebugSphere(GetWorld(), LineTraceLocation, 12.f, 8, FColor::Green, false, 1, 0, 1.f);
	DestroyGhostCam();
}



