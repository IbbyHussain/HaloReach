// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/GameModes/C_ReachGameStateBase.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetMathLibrary.h"
#include "HaloReach/UI/HUD/C_PlayerHUD.h"
#include "HaloReach/Player/C_PlayerCharacter.h"
#include "HaloReach/Player/PlayerExtra/C_ReachPlayerState.h"
#include "HaloReach/Player/PlayerExtra/C_ReachPlayerController.h"


AC_ReachGameStateBase::AC_ReachGameStateBase()
{
	
}

void AC_ReachGameStateBase::BeginPlay()
{
	Super::BeginPlay();
}

void AC_ReachGameStateBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AC_ReachGameStateBase::UpdateGlobalDeaths(FString A, FString B)
{
	for(auto x : PlayerArray)
	{
		AC_PlayerCharacter* Player = Cast<AC_PlayerCharacter>(x->GetPawn());
		if(Player)
		{
			AC_ReachPlayerController* RPC = Cast<AC_ReachPlayerController>(Player->GetController());
			if(RPC)
			{
				AC_PlayerHUD* HUD = Cast<AC_PlayerHUD>(RPC->GetHUD());
				if(HUD)
				{
					HUD->UpdateDeathWidget(A, B);
				}
			}
		}
	}
}


