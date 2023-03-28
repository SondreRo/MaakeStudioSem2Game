// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

struct FInputActionValue;


UCLASS()
class MAAKESTUDIO_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

	
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerVariables")
	//	class USpringArmComponent* SpringArm{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerVariables")
		class UCameraComponent* Camera{ nullptr };



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InputSystem)
	class UInputMappingContext* InputMapping;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InputSystem)
	class UInputAction* MovementForwardBackInput;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InputSystem)
	class UInputAction* MovementRightLeftInput;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InputSystem)
	class UInputAction* CameraPitchInput;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InputSystem)
	class UInputAction* CameraYawInput;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InputSystem)
	class UInputAction* JumpInput;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InputSystem)
	class UInputAction* MainActionInput;

	//Keyboard Number Buttons
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InputSystem)
		class UInputAction* SwapToolOneInput;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InputSystem)
		class UInputAction* SwapToolTwoInput;




	//Ghost Camera
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"));
	TSubclassOf<AActor> GhostCamera;

	//Player Placed Camera
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"));
	TSubclassOf<AActor> PlayerCamera;


private:
	void MovementForwardBack(const FInputActionValue& input);
	void MovementRightLeft(const FInputActionValue& input);

	void CameraPitch(const FInputActionValue& input);
	void CameraYaw(const FInputActionValue& input);

	void JumpTrigger(const FInputActionValue& input);

	void MainInteractTrigger(const FInputActionValue& input);
	void MainInteractEnd(const FInputActionValue& input);

	//Keyboard Number Buttons

	void SwapToolOne(const FInputActionValue& input);
	void SwapToolTwo(const FInputActionValue& input);



	void CharMovement();
	void DestroyGhostCam();
	void PlaceGhostCamera();
	void CameraPlaceMode();

	bool CheckCameraPlacement(FVector HitLocation);

	bool HoldingInteractButton;

	AActor* SpawnedGhostCamera;
	AActor* SpawnedPlayerCamera;

	TArray<AActor*> SpawnedPlayerCameraArray;
public:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerVariables | Animation")
		float InputX;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerVariables | Animation")
		float InputY;

	UPROPERTY(EditAnywhere, Category = "Collision")
		TEnumAsByte<ECollisionChannel> TraceChannelProperty = ECC_Pawn;

	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Variables")
		float RayLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Variables")
		float CameraMinDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Variables")
		int ToolSelected;



	float Pitch;
	float Yaw;

	FVector LineTraceLocation;
	FRotator LineTraceNormal;
	bool LineTraceHitSomething;


	


};
