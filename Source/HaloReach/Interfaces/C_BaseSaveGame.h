// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "C_BaseSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class HALOREACH_API UC_BaseSaveGame : public USaveGame
{
	GENERATED_BODY()

public:

	UC_BaseSaveGame();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Basic")
	FString SavedPlayerName;

	UPROPERTY(VisibleAnywhere, Category = "Basic")
	FString SaveSlotName;

	UPROPERTY(VisibleAnywhere, Category = "Basic")
	uint32 UserIndex;
	
};
