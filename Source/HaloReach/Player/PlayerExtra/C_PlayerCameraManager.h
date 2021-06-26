// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "C_PlayerCameraManager.generated.h"

/**
 * 
 */
UCLASS()
class HALOREACH_API AC_PlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

	AC_PlayerCameraManager();

public:
	void SetViewPitchAmount(float MinPitch, float MaxPitch);
};
