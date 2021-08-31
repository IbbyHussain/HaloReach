// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "C_ReachPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class HALOREACH_API AC_ReachPlayerState : public APlayerState
{
	GENERATED_BODY()

public:

	AC_ReachPlayerState();

	virtual void BeginPlay() override;
};
