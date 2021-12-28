// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/GameModes/C_ReachGameStateBase.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetMathLibrary.h"
#include "HaloReach/UI/HUD/C_PlayerHUD.h"
#include "HaloReach/Player/C_PlayerCharacter.h"
#include "HaloReach/Player/PlayerExtra/C_ReachPlayerState.h"
#include "HaloReach/Player/PlayerExtra/C_ReachPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "HaloReach/GameModes/C_ReachPlayerStart.h"
#include "EngineUtils.h"
#include <Runtime/Engine/Public/Net/UnrealNetwork.h>


AC_ReachGameStateBase::AC_ReachGameStateBase()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AC_ReachGameStateBase::BeginPlay()
{
	Super::BeginPlay();

	AC_ReachPlayerStart* PlayerStart;

	UWorld* World = GetWorld();
	for (TActorIterator<AC_ReachPlayerStart> It(World, AC_ReachPlayerStart::StaticClass()); It; ++It)
	{
		PlayerStart = *It;
		if (PlayerStart != NULL)
		{
			PlayerStartArray.AddUnique(PlayerStart);
		}
	}
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

void AC_ReachGameStateBase::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AC_ReachGameStateBase, MatchMinutes);
	DOREPLIFETIME(AC_ReachGameStateBase, MatchSeconds);

}




