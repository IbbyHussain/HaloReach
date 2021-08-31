// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/Player/PlayerExtra/C_ReachPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "HaloReach/Gamemodes/C_BaseReachGameMode.h"
#include "Haloreach/Player/C_PlayerCharacter.h"
#include "HaloReach/UI/HUD/C_PlayerHUD.h"
#include "GameFramework/PlayerState.h"


AC_ReachPlayerController::AC_ReachPlayerController()
{
	AssignedName = ("Not PC assigned");
}

void AC_ReachPlayerController::BeginPlay()
{
	Super::BeginPlay();

	AC_PlayerCharacter* PlayerCharacter = Cast<AC_PlayerCharacter>(GetPawn());
	if(PlayerCharacter)
	{
		PlayerCharacter->RespawnPlayer.AddDynamic(this, &AC_ReachPlayerController::RespawnPlayer);
	}
}

void AC_ReachPlayerController::RespawnPlayer(AC_PlayerCharacter* PlayerToRespawn)
{
	if(HasAuthority())
	{
		//if(GetPawn())
		//{
		//	// Still possesing a pawn
		//	Server_RequestGMRespawn(Player);
		//}

		//else
		//{
		//	// Not possessing a pawn
		//}

		AC_BaseReachGameMode* GM = Cast<AC_BaseReachGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
		if (GM)
		{
			GM->Server_RespawnPlayer(PlayerToRespawn);
		}
	}

	else
	{
		Server_RequestGMRespawn(PlayerToRespawn);
	}
}

void AC_ReachPlayerController::Server_RequestGMRespawn_Implementation(AC_PlayerCharacter* PlayerToRespawn)
{
	if(HasAuthority())
	{
		AC_BaseReachGameMode* GM =  Cast<AC_BaseReachGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
		if(GM)
		{
			GM->Server_RespawnPlayer(PlayerToRespawn);
		}
	}
}
