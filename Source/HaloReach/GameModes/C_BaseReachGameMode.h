// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "C_BaseReachGameMode.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerScored, int, PlayerScore, FString, PlayerName);

class AC_PlayerCharacter;

UCLASS()
class HALOREACH_API AC_BaseReachGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:

	AC_BaseReachGameMode();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

public:

	// Iterates over all players
	TTuple<APlayerController* , AC_PlayerCharacter* > IterateOverPlayers();

	FOnPlayerScored OnPlayerScored;

	bool bHasPlayerWon;

	// Checks if a player orr team has won, can be customised for each gamemode
	UFUNCTION()
	void CheckWinCondition(int PlayerScore, FString PlayerName);

# pragma region Respawn Player

	UFUNCTION(Server, Reliable)
	void Server_RespawnPlayer(AC_PlayerCharacter* PlayerToRespawn);
	void Server_RespawnPlayer_Implementation(AC_PlayerCharacter* PlayerToRespawn);

	void CheckAnyPlayersAlive();

# pragma endregion

# pragma region Match Timer

	void UpdateMatchTimer();

	FTimerHandle MatchTimerHandle;

# pragma endregion
};
