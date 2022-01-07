// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/GameModes/TeamSlayer/C_TeamSlayerGamemode.h"
#include "HaloReach/GameModes/TeamSlayer/C_TeamSlayerGameStateBase.h"

void AC_TeamSlayerGamemode::CheckWinCondition(int PlayerScore, FString PlayerName, ETeam PlayerTeam)
{
	if (!bHasPlayerWon)
	{
		AC_TeamSlayerGameStateBase* GS = GetGameState<AC_TeamSlayerGameStateBase>();

		if(GS)
		{
			if(PlayerTeam == ETeam::RED)
			{
				GS->Server_UpdateRedTeamScore();
				if(GS->RedTeamScore == GS->WinScore)
				{
					GS->bGameOver = !(GS->bGameOver);
					GS->WinningTeam = ETeam::RED;
					GS->WinningScore = GS->RedTeamScore;
					GS->OnRep_GameOver();
					bHasPlayerWon = true; 
				}
			}

			else
			{
				GS->Server_UpdateBlueTeamScore();
				if (GS->BlueTeamScore == GS->WinScore)
				{
					GS->bGameOver = !(GS->bGameOver);
					GS->WinningTeam = ETeam::BLUE;
					GS->WinningScore = GS->BlueTeamScore;
					GS->OnRep_GameOver();
					bHasPlayerWon = true;
				}
			}
		}
	}
}