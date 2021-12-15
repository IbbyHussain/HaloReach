// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "C_TeamsComponent.generated.h"



UENUM()
enum class ETeam : uint8
{
	RED,
	BLUE,
	GREEN,
	YELLOW,
	ORANGE,
	PURPLE,
	CYAN,
	BLACK,
	NEUTRAL
};

// Used for iteration over enums, requires an enum value to be used as the last value(does not include this in iteration)
ENUM_RANGE_BY_COUNT(ETeam, ETeam::NEUTRAL)

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HALOREACH_API UC_TeamsComponent : public UActorComponent
{
	GENERATED_BODY()

	UC_TeamsComponent();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Team") //VisibleAnywhere
	ETeam Team;

	// Default Functions

	virtual void BeginPlay() override;

	// Teams

	UFUNCTION(BlueprintCallable, Category = "Teams")
	ETeam GetTeam() const
	{
		return Team;
	}

	UFUNCTION(BlueprintCallable, Category = "Teams")
	void SetTeam(ETeam NewTeam)
	{
		// clients set team on server and locally for damage 
		if(!GetOwner()->HasAuthority())
		{
			Server_SetTeam(NewTeam);
		}
	
		Team = NewTeam;

		UpdateOwnerColour();
	}

	void OnTeamChanged();

	UFUNCTION(Server, Reliable)
	void Server_SetTeam(ETeam NewTeam);
	void Server_SetTeam_Implementation(ETeam NewTeam);

	// Will change the owners colour, to the team colour
	UFUNCTION(BlueprintCallable, Category = "Teams")
	void UpdateOwnerColour();

	UFUNCTION(BlueprintCallable)
	TArray<ETeam> GetAllTeams();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Teams")
	TArray<ETeam> TeamsArray;

};
