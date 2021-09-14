// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/Player/PlayerExtra/C_ReachPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "HaloReach/Gamemodes/C_BaseReachGameMode.h"
#include "Haloreach/Player/C_PlayerCharacter.h"
#include "HaloReach/UI/HUD/C_PlayerHUD.h"
#include "GameFramework/PlayerState.h"
#include "HaloReach/GameModes/C_ReachGameStateBase.h"
#include "HaloReach/Player/C_PlayerCharacter.h"

// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("texthere: %f), x));

AC_ReachPlayerController::AC_ReachPlayerController()
{
	AssignedName = ("Not PC assigned");
}

void AC_ReachPlayerController::BeginPlay()
{
	Super::BeginPlay();

	HUD = Cast<AC_PlayerHUD>(GetHUD());

	BindRespawnDelegate();
}

void AC_ReachPlayerController::BindRespawnDelegate()
{
	AC_PlayerCharacter* PlayerCharacter = Cast<AC_PlayerCharacter>(GetPawn());
	if (PlayerCharacter)
	{
		if(PlayerCharacter->RespawnPlayer.IsBound())
		{
			PlayerCharacter->RespawnPlayer.RemoveDynamic(this, &AC_ReachPlayerController::RespawnPlayer);
		}

		PlayerCharacter->RespawnPlayer.AddDynamic(this, &AC_ReachPlayerController::RespawnPlayer);
	}
}

// not being called on second death
void AC_ReachPlayerController::RespawnPlayer(AC_PlayerCharacter* PlayerToRespawn)
{
	if (PlayerToRespawn)
	{
		if (HUD)
		{
			//HUD->PlayHUDFadeOutAnimation();
			HUD->DestroyDeathWidget();
			HUD->CreateHUDWidget();
		}

		if (HasAuthority())
		{
			UnPossess();
			PlayerToRespawn->Destroy();

			//Spawn new player and possess
			FActorSpawnParameters SpawnParams;
			AC_PlayerCharacter* NewPlayerCharacter = GetWorld()->SpawnActor<AC_PlayerCharacter>(PlayerClass, FVector(0.0f, 0.0f, 200.0f), FRotator::ZeroRotator, SpawnParams);
			Possess(NewPlayerCharacter);

			BindRespawnDelegate();
		}

		else
		{
			Server_PossessPlayer(PlayerToRespawn);
		}
	}
}

void AC_ReachPlayerController::Server_PossessPlayer_Implementation(AC_PlayerCharacter* PlayerToRespawn)
{
	UnPossess();
	PlayerToRespawn->Destroy();

	//Spawn new player and possess
	FActorSpawnParameters SpawnParams;
	AC_PlayerCharacter* NewPlayerCharacter = GetWorld()->SpawnActor<AC_PlayerCharacter>(PlayerClass, FVector(0.0f, 0.0f, 200.0f), FRotator::ZeroRotator, SpawnParams);
	Possess(NewPlayerCharacter);
}

void AC_ReachPlayerController::Client_Bind_Implementation()
{
	BindRespawnDelegate();
}
