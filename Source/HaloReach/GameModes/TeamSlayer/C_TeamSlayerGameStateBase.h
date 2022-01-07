// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HaloReach/GameModes/C_ReachGameStateBase.h"
#include "HaloReach/GlobalEnums.h"
#include "C_TeamSlayerGameStateBase.generated.h"

class AC_ReachPlayerController;

UCLASS()
class HALOREACH_API AC_TeamSlayerGameStateBase : public AC_ReachGameStateBase
{
	GENERATED_BODY()

	AC_TeamSlayerGameStateBase();

public:

	UPROPERTY(Replicated, BlueprintReadWrite)
	TArray<AC_ReachPlayerController*>RedTeamArray;

	UPROPERTY(Replicated, BlueprintReadWrite)
	TArray<AC_ReachPlayerController*>BlueTeamArray;

	UPROPERTY(Replicated, BlueprintReadWrite)
	int RedTeamScore;

	UPROPERTY(Replicated, BlueprintReadWrite)
	int BlueTeamScore;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_UpdateRedTeamScore();
	void Server_UpdateRedTeamScore_Implementation();

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_UpdateBlueTeamScore();
	void Server_UpdateBlueTeamScore_Implementation();

	UPROPERTY(Replicated, BlueprintReadWrite)
	ETeam WinningTeam;

	UPROPERTY(Replicated, BlueprintReadWrite)
	int32 WinningScore;

	
};
