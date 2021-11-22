// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HaloReach/C_DirectionStruct.h"
#include "C_DirectionWidget.generated.h"

/**
 * 
 */
UCLASS()
class HALOREACH_API UC_DirectionWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	FDirections Directions;
	
};
