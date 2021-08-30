// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/GameModes/C_BaseReachGameMode.h"
#include "C_BaseReachGameMode.h"
#include "HaloReach/Player/C_PlayerCharacter.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "HaloReach/Components/C_HealthComponent.h"

#include "GameFramework/Actor.h"
#include "HaloReach/UI/HUD/C_PlayerHUD.h"


AC_BaseReachGameMode::AC_BaseReachGameMode()
{

}

void AC_BaseReachGameMode::BeginPlay()
{

}

//AActor* AC_BaseReachGameMode::ChoosePlayerStart_Implementation(AController* Player)
//{
//	return nullptr;
//}

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

	/*UE_LOG(LogTemp, Error, TEXT("GAME MODE RESPAWN"));

	TTuple<APlayerController*, AC_PlayerCharacter*> PlayerData = IterateOverPlayers();
	if(PlayerData.Get<1>()->bIsDead)
	{
		UE_LOG(LogTemp, Error, TEXT("DEAD Player name: %s"), *PlayerData.Get<1>()->GetName());
		//PC->UnPossess();
	//			UE_LOG(LogTemp, Error, TEXT("Player name: %s") *PlayerCharacter->GetName());
	//			// Spawn new player and possess
	//			//FActorSpawnParameters SpawnParams;
	//			//AC_PlayerCharacter* NewPlayerCharacter = GetWorld()->SpawnActor<AC_PlayerCharacter>(DefaultPawnClass, FVector(0.0f, 0.0f, 200.0f), FRotator::ZeroRotator, SpawnParams);
	//			//PC->Possess(NewPlayerCharacter);
	}*/

	UE_LOG(LogTemp, Error, TEXT("DEAD Player name: %s"), *PlayerToRespawn->GetName());
}



void AC_BaseReachGameMode::SetPlayerNames()
{
	TTuple<APlayerController*, AC_PlayerCharacter*> PlayerData = IterateOverPlayers();

}