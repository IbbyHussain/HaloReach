// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/GameModes/C_BaseReachGameMode.h"
#include "C_BaseReachGameMode.h"
#include "HaloReach/Player/C_PlayerCharacter.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "HaloReach/Components/C_HealthComponent.h"


AC_BaseReachGameMode::AC_BaseReachGameMode()
{

}

void AC_BaseReachGameMode::BeginPlay()
{
	// Bind delegate
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (PC && PC->GetPawn())
		{
			AC_PlayerCharacter* PlayerCharacter = Cast<AC_PlayerCharacter>(PC->GetPawn());
			if(PlayerCharacter)
			{
				PlayerCharacter->RespawnPlayer.AddDynamic(this, &AC_BaseReachGameMode::RespawnPlayer);
			}
		}
	}
}

//AActor* AC_BaseReachGameMode::ChoosePlayerStart_Implementation(AController* Player)
//{
//	return nullptr;
//}

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

void AC_BaseReachGameMode::RespawnPlayer()
{
	// Basic , temp respawn only works for server

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
}
