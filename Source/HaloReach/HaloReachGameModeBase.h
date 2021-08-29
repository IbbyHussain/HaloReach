// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HaloReachGameModeBase.generated.h"



UCLASS()
class HALOREACH_API AHaloReachGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:

	AHaloReachGameModeBase();

protected:

	float MatchTime;

	float CurrentMatchTime;

	FString GameModeName;

public:

	float GetMatchTime() const { return CurrentMatchTime; }

	FString GetMatchName() const { return GameModeName; }
	
};
