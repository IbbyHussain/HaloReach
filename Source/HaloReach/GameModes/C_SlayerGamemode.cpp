// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/GameModes/C_SlayerGamemode.h"
#include "HaloReach/Player/PlayerExtra/C_ReachPlayerState.h"
#include "Kismet/KismetMathLibrary.h"
#include "HaloReach/GlobalEnums.h"

AC_SlayerGamemode::AC_SlayerGamemode()
{
	PrimaryActorTick.bCanEverTick = true;

	PlayerTeamIndex = { 0,1,2,3,4,5,6,7 }; // Find better way to assign number of teams to this array ,3,4,5,6,7
}

void AC_SlayerGamemode::BeginPlay()
{
	Super::BeginPlay();


}

void AC_SlayerGamemode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

void AC_SlayerGamemode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	// weird bug where sometimes players can still be assigned same team, even if team is not inside array
	SetPlayerTeams(NewPlayer);
	
	// V1
	/*uint8 RandomNum = UKismetMathLibrary::RandomIntegerInRange(0, (uint8)ETeam::NEUTRAL - 1);
	ETeam PlayerTeam = (ETeam)RandomNum;
	PS->SetPlayerTeam(PlayerTeam);*/
}

void AC_SlayerGamemode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	// Adds player team back into array so another player can use it
	AC_ReachPlayerState* PS = Exiting->GetPlayerState<AC_ReachPlayerState>();
	if(PS)
	{
		PlayerTeamIndex.AddUnique(PS->GMTeamIndex);
	}
}

void AC_SlayerGamemode::SetPlayerTeams(APlayerController* NewPlayer)
{
	AC_ReachPlayerState* PS = NewPlayer->GetPlayerState<AC_ReachPlayerState>();
	if (PS)
	{
		uint8 RandomNum = UKismetMathLibrary::RandomIntegerInRange(PlayerTeamIndex[0], PlayerTeamIndex.Num() - 1);
		PS->GMTeamIndex = RandomNum;
		PlayerTeamIndex.Remove(RandomNum);
		ETeam PlayerTeam = (ETeam)RandomNum;
		PS->SetPlayerTeam(PlayerTeam);
	}
}


