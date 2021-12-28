// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/GameModes/C_SlayerGamemode.h"
#include "HaloReach/Player/PlayerExtra/C_ReachPlayerState.h"
#include "Kismet/KismetMathLibrary.h"
#include "HaloReach/GlobalEnums.h"

AC_SlayerGamemode::AC_SlayerGamemode()
{
	PlayerTeamIndex = { 0,1,2,3,4,5,6,7 }; // Find better way to assign number of teams to this array
}

void AC_SlayerGamemode::BeginPlay()
{
	Super::BeginPlay();


}

void AC_SlayerGamemode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	AC_ReachPlayerState* PS = NewPlayer->GetPlayerState<AC_ReachPlayerState>();
	if(PS)
	{

		// V1
		/*uint8 RandomNum = UKismetMathLibrary::RandomIntegerInRange(0, (uint8)ETeam::NEUTRAL - 1);
		ETeam PlayerTeam = (ETeam)RandomNum;
		PS->SetPlayerTeam(PlayerTeam);*/

		// V2
		uint8 RandomNum = UKismetMathLibrary::RandomIntegerInRange(PlayerTeamIndex[0], PlayerTeamIndex.Num() - 1);
		ETeam PlayerTeam = (ETeam)RandomNum;
		PS->SetPlayerTeam(PlayerTeam);
		PlayerTeamIndex.Remove(RandomNum);

		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Post login")));
	}
}