// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "HaloReach/GlobalEnums.h"
#include "C_ReachPlayerState.generated.h"

UCLASS()
class HALOREACH_API AC_ReachPlayerState : public APlayerState
{
	GENERATED_BODY()

public:

	AC_ReachPlayerState();

	virtual void BeginPlay() override;

	virtual void Tick(float Delta) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Team")
	ETeam PlayerTeam;

	float number;

	void Test()
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("PLAYER STATE FUNCTION WAS CALLED")));
	}
	 
	UFUNCTION(BlueprintCallable)
	ETeam GetPlayerTeam() const
	{
		return PlayerTeam;
	}

	void SetPlayerTeam(ETeam NewTeam)
	{
		if (!GetPawn()->HasAuthority())
		{
			Server_SetPlayerTeam(NewTeam);
		}

		PlayerTeam = NewTeam;
		Client_SetPlayerTeam(NewTeam);
	}

	UFUNCTION(Client, Reliable)
	void Client_SetPlayerTeam(ETeam NewTeam);
	void Client_SetPlayerTeam_Implementation(ETeam NewTeam);

	UFUNCTION(Server, Reliable)
	void Server_SetPlayerTeam(ETeam NewTeam);
	void Server_SetPlayerTeam_Implementation(ETeam NewTeam);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
	class AC_PlayerCharacter* PlayerOwner;

	// GM uses this to track team, in gm team array
	uint8 GMTeamIndex;

	//Replicated, 
	UPROPERTY(Replicated, BlueprintReadWrite)
	int PlayerScore;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};
