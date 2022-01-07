// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/GameModes/TeamSlayer/C_TeamSlayerGameStateBase.h"
#include "HaloReach/Player/PlayerExtra/C_ReachPlayerController.h"
#include "HaloReach/Player/PlayerExtra/C_ReachPlayerState.h"
#include "Net/UnrealNetwork.h"

AC_TeamSlayerGameStateBase::AC_TeamSlayerGameStateBase()
{
	GameModeName = "Slayer";

	MatchMinutes = 12;
	MatchSeconds = 59;

	WinScore = 25;
}

void AC_TeamSlayerGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AC_TeamSlayerGameStateBase, BlueTeamArray);
	DOREPLIFETIME(AC_TeamSlayerGameStateBase, RedTeamArray);

	DOREPLIFETIME(AC_TeamSlayerGameStateBase, RedTeamScore);
	DOREPLIFETIME(AC_TeamSlayerGameStateBase, BlueTeamScore);

	DOREPLIFETIME(AC_TeamSlayerGameStateBase, WinningTeam);
	DOREPLIFETIME(AC_TeamSlayerGameStateBase, WinningScore);

}

void AC_TeamSlayerGameStateBase::Server_UpdateRedTeamScore_Implementation()
{
	RedTeamScore = 0;

	for (auto r : RedTeamArray)
	{
		RedTeamScore += r->GetPlayerState<AC_ReachPlayerState>()->GetPlayerScore();
	}
}

void AC_TeamSlayerGameStateBase::Server_UpdateBlueTeamScore_Implementation()
{
	BlueTeamScore = 0;

	for (auto r : BlueTeamArray)
	{
		BlueTeamScore += r->GetPlayerState<AC_ReachPlayerState>()->GetPlayerScore();
	}
}
