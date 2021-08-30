// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "C_ReachGameState.generated.h"

/**
 * 
 */
UCLASS()
class HALOREACH_API AC_ReachGameState : public AGameState
{
	GENERATED_BODY()
	
public:

	AC_ReachGameState();

	virtual void BeginPlay() override;
};
