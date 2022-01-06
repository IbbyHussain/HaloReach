// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/GameModes/C_BaseReachGameMode.h"
#include "C_BaseReachGameMode.h"
#include "HaloReach/Player/C_PlayerCharacter.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "HaloReach/Components/C_HealthComponent.h"

#include "GameFramework/Actor.h"
#include "Kismet/KismetMathLibrary.h"
#include "HaloReach/UI/HUD/C_PlayerHUD.h"
#include "HaloReach/Player/PlayerExtra/C_ReachPlayerState.h"
#include "HaloReach/GameModes/C_ReachGameStateBase.h"


AC_BaseReachGameMode::AC_BaseReachGameMode()
{
	PrimaryActorTick.bCanEverTick = true;

	PlayerStateClass = AC_ReachPlayerState::StaticClass();
}

void AC_BaseReachGameMode::BeginPlay()
{
	Super::BeginPlay();

	OnPlayerScored.AddDynamic(this, &AC_BaseReachGameMode::CheckWinCondition);

	GetWorldTimerManager().SetTimer(MatchTimerHandle, this, &AC_BaseReachGameMode::UpdateMatchTimer, 1.0f, true);
}

void AC_BaseReachGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AC_BaseReachGameMode::UpdateMatchTimer()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("1")));
	AC_ReachGameStateBase* GS = GetGameState<AC_ReachGameStateBase>();
	if(GS)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("2")));
		if(GS->GetMatchSeconds() == 0)
		{
			if(GS->GetMatchMinutes() == 0)
			{
				// On rep can be used to communicate with gamestate
				GS->bGameOver = !(GS->bGameOver);
				GS->OnRep_GameOver();
			}

			else
			{
				GS->SetMatchSeconds(59);
				GS->SetMatchMinutes(GS->GetMatchMinutes() - 1);
			}
		}

		else
		{
			GS->SetMatchSeconds(GS->GetMatchSeconds() - 1);
		}
	}
}

TTuple<APlayerController*, AC_PlayerCharacter*> AC_BaseReachGameMode::IterateOverPlayers()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (PC && PC->GetPawn())
		{
			AC_PlayerCharacter* PlayerCharacter = Cast<AC_PlayerCharacter>(PC->GetPawn());
			if (PlayerCharacter)
			{
				return MakeTuple(PC, PlayerCharacter);
			}
		}
	}

	return MakeTuple(nullptr, nullptr);
}

void AC_BaseReachGameMode::CheckWinCondition(int PlayerScore, FString PlayerName)
{
	if(!bHasPlayerWon)
	{
		AC_ReachGameStateBase* GS = GetGameState<AC_ReachGameStateBase>();
		if (GS && PlayerScore >= GS->GetWinScore())
		{
			if (GS)
			{
				GS->bGameOver = !(GS->bGameOver);
				GS->OnRep_GameOver();
				bHasPlayerWon = true; //Only one player can win
			}
		}
	}
}

void AC_BaseReachGameMode::CheckAnyPlayersAlive()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* Pc = It->Get();
		if(Pc && Pc->GetPawn())
		{
			APawn* MyPawn = Pc->GetPawn();
			UC_HealthComponent* HealthComp = Cast<UC_HealthComponent>(MyPawn->GetComponentByClass(UC_HealthComponent::StaticClass()));

			if (ensure(HealthComp) && HealthComp->GetHealth() > 0.0f)
			{
				// Player is alive
				return;
			}
		}
	}

	// No players are alive
}

void AC_BaseReachGameMode::Server_RespawnPlayer_Implementation(AC_PlayerCharacter* PlayerToRespawn)
{
	// Basic , temp respawn only works for server
	//GEngine->AddOnScreenDebugMessage(-1, 4.5f, FColor::Magenta, FString::Printf(TEXT("DEAD PLAYER IS: %s"), *PlayerToRespawn->GetName()));

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (PC && PC->GetPawn())
		{
			AC_PlayerCharacter* PlayerCharacter = Cast<AC_PlayerCharacter>(PC->GetPawn());
			if (PlayerCharacter && PlayerCharacter->bIsDead)
			{
				PlayerCharacter->Destroy();
				PC->UnPossess();

				// Spawn new player and possess
				FActorSpawnParameters SpawnParams;
				AC_PlayerCharacter* NewPlayerCharacter = GetWorld()->SpawnActor<AC_PlayerCharacter>(DefaultPawnClass, FVector(0.0f, 0.0f, 200.0f), FRotator::ZeroRotator, SpawnParams);
				PC->Possess(NewPlayerCharacter);
			}
		}
	}
	
	

	//UE_LOG(LogTemp, Error, TEXT("GAME MODE RESPAWN"));

	/*TTuple<APlayerController*, AC_PlayerCharacter*> PlayerData = IterateOverPlayers();
	if(PlayerData.Get<1>()->bIsDead)
	{
		UE_LOG(LogTemp, Error, TEXT("DEAD Player name: %s"), *PlayerData.Get<1>()->GetName());

		PlayerData.Get<0>()->UnPossess();

		//Spawn new player and possess
		FActorSpawnParameters SpawnParams;
		AC_PlayerCharacter* NewPlayerCharacter = GetWorld()->SpawnActor<AC_PlayerCharacter>(DefaultPawnClass, FVector(0.0f, 0.0f, 200.0f), FRotator::ZeroRotator, SpawnParams);
		PlayerData.Get<0>()->Possess(NewPlayerCharacter);
	}*/

	
}

