// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HaloReach/GameModes/C_ReachGameStateBase.h"
#include "C_SlayerGameStateBase.generated.h"

class AC_ReachPlayerState;

UCLASS()
class HALOREACH_API AC_SlayerGameStateBase : public AC_ReachGameStateBase
{
	GENERATED_BODY()

public:
	AC_SlayerGameStateBase();

	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite)
	TArray <int32> PlayerTeamIndexArray;

	int32 GetRandomTeamIndex();

	int32 TeamCounter = -1;

	void ShuffleArray();

	void AssignRandomTeam(AC_ReachPlayerState* ReachPlayerState);
};
