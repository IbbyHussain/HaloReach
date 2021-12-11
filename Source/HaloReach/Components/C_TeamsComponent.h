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
	WHITE,
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
		Team = NewTeam;

		UpdateOwnerColour();
	}

	void OnTeamChanged();

	// Will change the owners colour, to the team colour
	UFUNCTION(BlueprintCallable, Category = "Teams")
	void UpdateOwnerColour();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Teams")
	TArray<FString> TeamsArray;

	UFUNCTION(BlueprintCallable)
	TArray<FString> GetAllTeams();

	// testings 

	UFUNCTION(BlueprintCallable)
	TArray<ETeam> GetAllTeamsENUM();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Teams")
	TArray<ETeam> TeamsENUMArray;

};
