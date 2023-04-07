// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerCamera.generated.h"

UCLASS()
class MAAKESTUDIO_API APlayerCamera : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlayerCamera();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerVariables")
	class UCameraComponent* Camera{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerVariables")
	class USphereComponent* SphereCollider{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerVariables")
		class UStaticMeshComponent* Indicator{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyVeriables")
	UStaticMeshComponent* StaticMesh{ nullptr };
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Materials")
	UMaterial* SelectedMaterial;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Materials")
	UMaterial* NormalMaterial;

	UFUNCTION(BlueprintCallable)
	void CamSelected();
	UFUNCTION(BlueprintCallable)
	void CamDeselected();

};
