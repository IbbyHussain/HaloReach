// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "C_ReachPlayerStart.generated.h"


UCLASS()
class HALOREACH_API AC_ReachPlayerStart : public APlayerStart
{
	GENERATED_BODY()


public:

	AC_ReachPlayerStart(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditDefaultsOnly, Category = "PlayerStart | Components")
	class UCameraComponent* CameraComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PlayerStart | Components")
	class USpringArmComponent* SpringArmComp;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "PlayerStart | Components")
	FName LocationName;

	// Will return true if the player start is unoccupied
	bool bIsSuitable();

	// The radius of the sphere trace which checks if this player start is vacant
	UPROPERTY(EditDefaultsOnly, Category = "PlayerStart | Attributes")
	float SphereRadius;


	
};
