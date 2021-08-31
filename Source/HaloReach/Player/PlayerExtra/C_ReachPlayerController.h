// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "C_ReachPlayerController.generated.h"


class AC_PlayerCharacter;

UCLASS()
class HALOREACH_API AC_ReachPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	AC_ReachPlayerController();

	virtual void BeginPlay() override;

	UFUNCTION()
	void RespawnPlayer(AC_PlayerCharacter* PlayerToRespawn);

	UFUNCTION(Server, Reliable)
	void Server_RequestGMRespawn(AC_PlayerCharacter* PlayerToRespawn);
	void Server_RequestGMRespawn_Implementation(AC_PlayerCharacter* PlayerToRespawn);

	UPROPERTY(BlueprintReadWrite)
	FString AssignedName;

};
