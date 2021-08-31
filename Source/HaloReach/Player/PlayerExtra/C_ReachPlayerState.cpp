// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/Player/PlayerExtra/C_ReachPlayerState.h"
#include "HaloReach/UI/HUD/C_PlayerHUD.h"
#include "HaloReach/Player/PlayerExtra/C_ReachPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "HaloReach/Gamemodes/C_BaseReachGameMode.h"
#include "Kismet/KismetMathLibrary.h"

#include "HaloReach/GameModes/C_ReachGameStateBase.h"


AC_ReachPlayerState::AC_ReachPlayerState()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AC_ReachPlayerState::BeginPlay()
{
	Super::BeginPlay();

	AC_ReachGameStateBase* GS = Cast<AC_ReachGameStateBase>(UGameplayStatics::GetGameState(GetWorld()));

	if(GS)
	{
		
		//AC_PlayerHUD* HUD = Cast<AC_PlayerHUD>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD());
		//HUD->SetPlayerNameTextHUD(GetPlayerName()); //GS->PlayerNamesArray[UKismetMathLibrary::RandomIntegerInRange(0, GS->PlayerNamesArray.Num() - 1)]
		//	AC_ReachPlayerController* PC = Cast<AC_ReachPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		//SetPlayerName(PC->AssignedName);
	}
}

void AC_ReachPlayerState::Tick(float Delta)
{
	Super::Tick(Delta);

	AC_PlayerHUD* HUD = Cast<AC_PlayerHUD>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD());
	AC_ReachPlayerController* PC = Cast<AC_ReachPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	SetPlayerName(HUD->HUDTestname());


}


