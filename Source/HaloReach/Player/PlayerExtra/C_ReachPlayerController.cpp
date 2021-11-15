// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/Player/PlayerExtra/C_ReachPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "HaloReach/Gamemodes/C_BaseReachGameMode.h"
#include "Haloreach/Player/C_PlayerCharacter.h"
#include "HaloReach/UI/HUD/C_PlayerHUD.h"
#include "GameFramework/PlayerState.h"
#include "HaloReach/GameModes/C_ReachGameStateBase.h"
#include "HaloReach/Player/C_PlayerCharacter.h"
#include "HaloReach/GameModes/C_ReachPlayerStart.h"
#include "Kismet/KismetMathLibrary.h"
#include "HaloReach/Components/C_RadarIconComponent.h"

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
			HUD->PlaySpawnFadeIn();
		}

		// if player starts are occuppied wont spawn

		if (HasAuthority())
		{
			UnPossess();
			PlayerToRespawn->Destroy();

			//Spawn new player and possess
			FActorSpawnParameters SpawnParams;
			AC_PlayerCharacter* NewPlayerCharacter = GetWorld()->SpawnActor<AC_PlayerCharacter>(PlayerClass, GetPlayerSpawnLocation(), FRotator::ZeroRotator, SpawnParams);
			Possess(NewPlayerCharacter);

			NewPlayerCharacter->Server_SetPlayerName(AssignedName);

			Client_RefreshRadar();

			BindRespawnDelegate();

			BP_PlayerHasRespawned();
		}

		else
		{
			Server_PossessPlayer(PlayerToRespawn, AssignedName);
		}

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("NAME: %s"), *AssignedName));
	}
}

void AC_ReachPlayerController::Server_PossessPlayer_Implementation(AC_PlayerCharacter* PlayerToRespawn, const FString& PlayerName)
{
	UnPossess();
	PlayerToRespawn->Destroy();

	//Spawn new player and possess
	FActorSpawnParameters SpawnParams;
	AC_PlayerCharacter* NewPlayerCharacter = GetWorld()->SpawnActor<AC_PlayerCharacter>(PlayerClass, GetPlayerSpawnLocation(), FRotator::ZeroRotator, SpawnParams);
	Possess(NewPlayerCharacter);
	NewPlayerCharacter->Server_SetPlayerName(PlayerName);

	Client_RefreshRadar();

	BP_PlayerHasRespawned();
}

FVector AC_ReachPlayerController::GetPlayerSpawnLocation()
{
	AC_ReachGameStateBase* ReachGameState = Cast<AC_ReachGameStateBase>(UGameplayStatics::GetGameState(GetWorld()));

	TArray<AC_ReachPlayerStart*> SuitablePlayerStartArray;

	if (ReachGameState)
	{
		for (auto x : ReachGameState->PlayerStartArray)
		{
			if(x->bIsSuitable())
			{
				SuitablePlayerStartArray.AddUnique(x);
			}
		}

		if(SuitablePlayerStartArray.Num() != 0)
		{
			for (auto i : SuitablePlayerStartArray)
			{
				int Num = UKismetMathLibrary::RandomIntegerInRange(0, SuitablePlayerStartArray.Num() - 1);
				FVector PlayerSpawnLocation = SuitablePlayerStartArray[Num]->GetActorLocation();
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("SUITABLE RESPAWN CLIENT")));
				return PlayerSpawnLocation;
			}
		}

		else
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("NOT SUITABLE RESPAWN CLIENT")));
			return FVector(0, 0, 0);
		}
	}

	return FVector(0, 0, 0);
}

void AC_ReachPlayerController::Client_RefreshRadar_Implementation()
{
	AC_PlayerCharacter* PlayerCharacter = Cast<AC_PlayerCharacter>(GetPawn());
	if(PlayerCharacter)
	{
		PlayerCharacter->GetRadarComponent()->RefreshRadarIcons();
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Added icon")));
	}
}

void AC_ReachPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// MOVING
	//InputComponent->BindAction("Test", IE_Pressed, this, &AC_ReachPlayerController::SetPlayerSpawnLocation);
}
