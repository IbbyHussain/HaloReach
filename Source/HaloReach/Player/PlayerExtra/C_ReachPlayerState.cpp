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



AC_ReachPlayerState::AC_ReachPlayerState()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AC_ReachPlayerState::BeginPlay()
{
	Super::BeginPlay();
}

void AC_ReachPlayerState::Tick(float Delta)
{
	Super::Tick(Delta);

	AC_PlayerCharacter* PlayerCharacter = Cast<AC_PlayerCharacter>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetPawn());
	if(PlayerCharacter && PlayerCharacter->IsLocallyControlled())
	{
		UC_PlayerNameWidget* PlayerNameWidget = Cast<UC_PlayerNameWidget>(PlayerCharacter->PlayerNameWidgetComp->GetUserWidgetObject());
		if(PlayerNameWidget)
		{
			SetPlayerName(PlayerNameWidget->DisplayedPlayerName);
		}
	}
	
	
}




