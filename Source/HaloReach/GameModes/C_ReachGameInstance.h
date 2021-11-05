// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "C_ReachGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class HALOREACH_API UC_ReachGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	UC_ReachGameInstance();

	UFUNCTION(BlueprintCallable)
	FString LoadPlayerName();

	
	
};
