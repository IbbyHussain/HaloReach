// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "C_BaseReachGameMode.generated.h"

class AC_PlayerCharacter;

UCLASS()
class HALOREACH_API AC_BaseReachGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:

	AC_BaseReachGameMode();

	virtual void BeginPlay() override;

	//virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	UFUNCTION(BlueprintCallable)
	void RespawnPlayer();

	void CheckAnyPlayersAlive();
	
};
