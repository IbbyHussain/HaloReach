// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/GameModes/C_ReachGameInstance.h"
#include "HaloReach/Interfaces/C_BaseSaveGame.h"
#include "Kismet/GameplayStatics.h"

UC_ReachGameInstance::UC_ReachGameInstance()
{

}

FString UC_ReachGameInstance::LoadPlayerName()
{
	if (UC_BaseSaveGame* LoadedGame = Cast<UC_BaseSaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("NAME"), 0)))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("LOAD GAME SUCCEEDED (player name)")));

		return LoadedGame->SavedPlayerName;
	}

	return FString("fail");
}



