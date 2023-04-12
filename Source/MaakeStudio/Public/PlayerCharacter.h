// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

struct FInputActionValue;

class APlayerSideCharacter;

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
	class UInputAction* DeleteInput;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InputSystem)
	class UInputAction* RunInput;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InputSystem)
	class UInputAction* MainActionInput;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InputSystem)
		class UInputAction* CameraModeToggleInput;

	//Keyboard Number Buttons
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InputSystem)
	class UInputAction* SwapToolOneInput;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InputSystem)
	class UInputAction* SwapToolTwoInput;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InputSystem)
	class UInputAction* ScrollWheelInput;


	UFUNCTION(BlueprintImplementableEvent, Category = "Calleble")
		void UpdateHand(int ItemSlot);


	//Ghost Camera
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"));
	TSubclassOf<AActor> GhostCamera;

	//Player Placed Camera
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"));
	TSubclassOf<AActor> PlayerCamera;


private:

	//---------------MovementInputs-------------------//
	void MovementForwardBack(const FInputActionValue& input);
	void MovementRightLeft(const FInputActionValue& input);

	void CameraPitch(const FInputActionValue& input);
	void CameraYaw(const FInputActionValue& input);

	void JumpTrigger(const FInputActionValue& input);

	void RunStart(const FInputActionValue& input);
	void RunEnd(const FInputActionValue& input);

	//---------------InteractionInputs-------------------//
	void MainInteractStarted(const FInputActionValue& input);
	void MainInteractTrigger(const FInputActionValue& input);
	void MainInteractEnd(const FInputActionValue& input);

	void DeleteTrigger(const FInputActionValue& input);

	//---------------InventoryButtonInputs-------------------//
	void SwapToolOne(const FInputActionValue& input);
	void SwapToolTwo(const FInputActionValue& input);

	void ScrollWheelTrigger(const FInputActionValue& input);

	//---------------CameraInputs-------------------//
	void CameraModeToggleTrigger(const FInputActionValue& input);

	void MovementRightLeftStarted(const FInputActionValue& input);



		
	//---------------MovementFunctions-------------------//
	void CharMovement();

	//---------------SelectFunctions-------------------//
	void SelectMode();

	//---------------CameraFunctions-------------------//
	void CameraPlaceMode();
	void PlaceGhostCamera();
	void DestroyGhostCam();
	bool CheckCameraPlacement(FVector HitLocation);

	void ChangeViewTarget(int CameraIndex);

	void ShootRayForSideCharacter();
	bool CheckSideCharacterLineOfSight(class APlayerCamera* CurrentCam);
	//---------------PrivateVariables-------------------//
	
	bool HoldingInteractButton;

	AActor * SpawnedGhostCamera;
	AActor* SpawnedPlayerCamera;
	APlayerController* PlayerController;
	TArray<AActor*> SpawnedPlayerCameraArray;


	




	template<typename T>
	void FindAllActors(UWorld* World, TArray<T*>& Out)
	{
		for (TActorIterator<T> It(World); It; ++It)
		{
			Out.Add(*It);
		}
	}

	TArray<APlayerSideCharacter*> AllActorsToControll;

public:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerVariables")
		float InputX;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerVariables")
		float InputY;

	UPROPERTY(EditAnywhere, Category = "Collision")
		TEnumAsByte<ECollisionChannel> TraceChannelProperty;

	UPROPERTY(EditAnywhere, Category = "Collision")
		TEnumAsByte<ECollisionChannel> SelectTraceChannelProperty = ECC_Pawn;
	
	UPROPERTY(EditAnywhere, Category = "Collision")
		TEnumAsByte<ECollisionChannel> SideCharacterRayProperty;

	

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Variables|CameraPlacement")
		float RayLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Variables|CameraPlacement")
		float SideCharacterRayLength;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Variables|CameraPlacement")
		float CameraMinDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Variables")
		int ToolSelected;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Variables|CameraPlacement")
		AActor* SelectedCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Variables|CameraPlacement")
		AActor* CurrentActiveCamera;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Variables|CameraPlacement")
		int MaxCameras;

	

	float Pitch;
	float Yaw;

	int CameraToChangeTo;
	bool CameraViewMode;


	FVector LineTraceLocation;
	FRotator LineTraceNormal;
	bool LineTraceHitSomething;
	bool IsRunning;

	float ScrollWheelValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Variables")
	float WalkSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Variables")
	float RunSpeed;

};
