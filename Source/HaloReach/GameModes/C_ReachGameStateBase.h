// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "C_ReachGameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class HALOREACH_API AC_ReachGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
public:

	AC_ReachGameStateBase();

	UPROPERTY(EditDefaultsOnly, Category = "Base Gamemode | Player Names")
		TArray<FString> Names;
};
