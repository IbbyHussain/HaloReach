// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HaloReach/GameModes/C_BaseReachGameMode.h"
#include "C_TeamSlayerGamemode.generated.h"

/**
 * 
 */
UCLASS()
class HALOREACH_API AC_TeamSlayerGamemode : public AC_BaseReachGameMode
{
	GENERATED_BODY()
	
public:

	virtual void CheckWinCondition(int PlayerScore, FString PlayerName, ETeam PlayerTeam) override;
};
