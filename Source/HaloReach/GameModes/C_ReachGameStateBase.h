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

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void UpdateGlobalDeaths(FString A, FString B);
};
