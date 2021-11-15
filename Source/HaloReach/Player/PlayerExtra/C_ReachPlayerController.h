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

	UPROPERTY(BlueprintReadWrite)
	FString AssignedName;

	UPROPERTY(EditDefaultsOnly, Category = "S")
	TSubclassOf<AC_PlayerCharacter> PlayerClass;

	UFUNCTION(Server, Reliable)
	void Server_PossessPlayer(AC_PlayerCharacter* PlayerToRespawn, const FString& PlayerName);
	void Server_PossessPlayer_Implementation(AC_PlayerCharacter* PlayerToRespawn, const FString& PlayerName);

	void BindRespawnDelegate();

	UPROPERTY()
	class AC_PlayerHUD* HUD;

	// Input

	virtual void SetupInputComponent() override;

# pragma region Player Spawn

	FVector GetPlayerSpawnLocation();

	// Used to esnure that the Refresh Radar Icons method is only called once and can be reset throughout the game as the controller is never destroyed.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "S")
	bool bHasRespawned;

	// Adds icons from other players to this players radar.
	UFUNCTION(Client, Reliable)
	void Client_RefreshRadar();
	void Client_RefreshRadar_Implementation();

# pragma endregion

	UFUNCTION(BlueprintImplementableEvent, Category = "BaseCharacter")
		void BP_PlayerHasRespawned();


};
