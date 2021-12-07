// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "C_TeamsComponent.generated.h"


UENUM()
enum class ETeam : uint8
{
	NEUTRAL,
	RED,
	BLUE,
	GREEN,
	YELLOW,
	ORANGE,
	PURPLE,
	WHITE,
	BLACK
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HALOREACH_API UC_TeamsComponent : public UActorComponent
{
	GENERATED_BODY()

	UC_TeamsComponent();

	UPROPERTY(VisibleAnywhere, Category = "Team")
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

};
