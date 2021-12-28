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


AC_BaseReachGameMode::AC_BaseReachGameMode()
{
	PlayerStateClass = AC_ReachPlayerState::StaticClass();
}

void AC_BaseReachGameMode::BeginPlay()
{
	Super::BeginPlay();
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



	GEngine->AddOnScreenDebugMessage(-1, 4.5f, FColor::Magenta, FString::Printf(TEXT("DEAD PLAYER IS: %s"), *PlayerToRespawn->GetName()));

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

//void AC_BaseReachGameMode::RespawnPlayer()
//{
//	// Basic , temp respawn only works for server
//
//	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
//	{
//		APlayerController* PC = It->Get();
//		if (PC && PC->GetPawn())
//		{
//			AC_PlayerCharacter* PlayerCharacter = Cast<AC_PlayerCharacter>(PC->GetPawn());
//			if (PlayerCharacter && PlayerCharacter->bIsDead)
//			{
//				PlayerCharacter->Destroy();
//				PC->UnPossess();
//
//				// Spawn new player and possess
//				FActorSpawnParameters SpawnParams;
//				AC_PlayerCharacter* NewPlayerCharacter = GetWorld()->SpawnActor<AC_PlayerCharacter>(DefaultPawnClass, FVector(0.0f, 0.0f, 200.0f), FRotator::ZeroRotator, SpawnParams);
//				PC->Possess(NewPlayerCharacter);
//			}
//		}
//	}
//}

void  AC_BaseReachGameMode::StartPlay()
{
	Super::StartPlay();

	//UE_LOG(LogTemp, Error, TEXT("Start Play called"));

	//GEngine->AddOnScreenDebugMessage(-1, 4.5f, FColor::Magenta, FString::Printf(TEXT("Start Play called")));

	
}