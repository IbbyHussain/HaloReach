// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/Player/PlayerExtra/C_ReachPlayerState.h"
#include "HaloReach/UI/HUD/C_PlayerHUD.h"
#include "Kismet/GameplayStatics.h"
#include "HaloReach/Gamemodes/C_BaseReachGameMode.h"
#include "Kismet/KismetMathLibrary.h"

#include "HaloReach/GameModes/C_ReachGameStateBase.h"
#include "Kismet/KismetMathLibrary.h"

AC_ReachPlayerState::AC_ReachPlayerState()
{

}

void AC_ReachPlayerState::BeginPlay()
{
	Super::BeginPlay();

	AC_ReachGameStateBase* GS = Cast<AC_ReachGameStateBase>(UGameplayStatics::GetGameState(GetWorld()));
	if(GS)
	{
		AC_PlayerHUD* HUD = Cast<AC_PlayerHUD>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD());
		HUD->SetPlayerNameTextHUD(GS->Names[UKismetMathLibrary::RandomIntegerInRange(0, GS->Names.Num() - 1)]);
	}
}

void AC_ReachPlayerState::Server_RequestGMName_Implementation()
{
	if(HasAuthority())
	{
		AC_BaseReachGameMode* GM = Cast<AC_BaseReachGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
		if(GM)
		{
			//SetPlayerName(GM->PlayerNamesArray[UKismetMathLibrary::RandomIntegerInRange(0, GM->PlayerNamesArray.Num() - 1)]);
		}
	}
}
