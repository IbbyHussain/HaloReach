// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/Player/PlayerExtra/C_ReachPlayerState.h"
#include "HaloReach/UI/HUD/C_PlayerHUD.h"
#include "HaloReach/Player/PlayerExtra/C_ReachPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "HaloReach/UI/C_PlayerNameWidget.h"
#include "HaloReach/Gamemodes/C_BaseReachGameMode.h"
#include "Kismet/KismetMathLibrary.h"
#include "HaloReach/Player/C_PlayerCharacter.h"
#include "HaloReach/GameModes/C_ReachGameStateBase.h"
#include "Components/WidgetComponent.h"
#include <Runtime/Engine/Public/Net/UnrealNetwork.h>




AC_ReachPlayerState::AC_ReachPlayerState()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AC_ReachPlayerState::BeginPlay()
{
	Super::BeginPlay();

	PlayerOwner = GetPawn<AC_PlayerCharacter>();
	PlayerOwner->Test();
}

void AC_ReachPlayerState::Tick(float Delta)
{
	Super::Tick(Delta);
}

void AC_ReachPlayerState::Client_SetPlayerTeam_Implementation(ETeam NewTeam)
{
	PlayerTeam = NewTeam;
}

void AC_ReachPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(AC_ReachPlayerState, PlayerScore);
	
}



