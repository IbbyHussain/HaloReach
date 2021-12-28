// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HaloReach/GameModes/C_BaseReachGameMode.h"
#include "C_SlayerGamemode.generated.h"



UCLASS()
class HALOREACH_API AC_SlayerGamemode : public AC_BaseReachGameMode
{
	GENERATED_BODY()

public:

	AC_SlayerGamemode();

	virtual void BeginPlay() override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

	TArray <uint8> PlayerTeamIndex;
	
};
