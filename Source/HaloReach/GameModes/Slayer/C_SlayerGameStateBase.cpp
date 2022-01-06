// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/GameModes/Slayer/C_SlayerGameStateBase.h"
#include "Kismet/KismetArrayLibrary.h"
#include "HaloReach/GlobalEnums.h"
#include "HaloReach/Player/PlayerExtra/C_ReachPlayerState.h"
#include "HaloReach/Player/PlayerExtra/C_ReachPlayerController.h"

AC_SlayerGameStateBase::AC_SlayerGameStateBase()
{
	PrimaryActorTick.bCanEverTick = true;

	GameModeName = "Slayer";

	MatchMinutes = 0;
	MatchSeconds = 15;

	WinScore = 5;

	PlayerTeamIndexArray = { 0,1,2,3,4,5,6,7 }; // Find better way to assign number of teams to this array ,4,5,6,7
}

void AC_SlayerGameStateBase::BeginPlay()
{
	Super::BeginPlay();

	ShuffleArray();

	for (auto i : PlayerTeamIndexArray)
	{
		UE_LOG(LogTemp, Warning, TEXT("%d"), i);
	}
}

void AC_SlayerGameStateBase::ShuffleArray()
{
	const int32 LastIndex = PlayerTeamIndexArray.Num() - 1;
	for (int32 i = 0; i < LastIndex; ++i)
	{
		int32 Index = FMath::RandRange(0, LastIndex);
		if (i != Index)
		{
			PlayerTeamIndexArray.Swap(i, Index);
		}
	}
}

int32 AC_SlayerGameStateBase::GetRandomTeamIndex()
{
	TeamCounter += 1;

	return PlayerTeamIndexArray[TeamCounter];
}

void AC_SlayerGameStateBase::AssignRandomTeam(AC_ReachPlayerState* ReachPlayerState)
{
	if(ReachPlayerState)
	{
		int32 RandomIndex = GetRandomTeamIndex();

		UE_LOG(LogTemp, Error, TEXT("%d"), RandomIndex);

		uint8 RandomPlayerTeam = uint8(RandomIndex);

		ETeam PlayerTeam = ETeam(RandomPlayerTeam);

		ReachPlayerState->SetPlayerTeam(PlayerTeam);
	}
}