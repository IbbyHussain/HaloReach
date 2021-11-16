// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "C_RadarIconComponent.generated.h"


UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class HALOREACH_API UC_RadarIconComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UC_RadarIconComponent();

	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, Category = "Components")
	AActor* Owner;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Components")
	UTexture2D* RadarIconImage;

	// Will add a radar icon to the players radar widget
	UFUNCTION(BlueprintCallable)
	void AddRadarIcon();

	class AC_PlayerHUD* HUD;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class UC_RadarIcon* RadarIcon;

	// Removes the owners radar icon from all players in game
	UFUNCTION(BlueprintCallable)
	void RemoveRadarIcon();

	UFUNCTION(Server, Reliable)
	void Server_RemoveRadarIcon();
	void Server_RemoveRadarIcon_Implementation();

	UFUNCTION(NetMulticast, Reliable)
	void Multi_RemoveRadarIcon();
	void Multi_RemoveRadarIcon_Implementation();

	// Gets all players and adds their icons to this players radar
	UFUNCTION(BlueprintCallable)
	void RefreshRadarIcons();

	UPROPERTY(EditDefaultsOnly, Category = "Radar")
	UTexture2D* TAllyIcon;

	UPROPERTY(EditDefaultsOnly, Category = "Radar")
	UTexture2D* TEnemyIcon;

# pragma region Radar Toggle

	// Used to set the owners radar icon on or off for other players. The owners local icon will always be visisble
	UFUNCTION(BlueprintCallable)
	void ToggleRadarIcon(bool bShowRadarIcon);

	UFUNCTION(Server, Reliable)
	void Server_ToggleRadarIcon(bool bShowRadarIcon);
	void Server_ToggleRadarIcon_Implementation(bool bShowRadarIcon);

	UFUNCTION(NetMulticast, Reliable)
	void Multi_ToggleRadarIcon(bool bShowRadarIcon);
	void Multi_ToggleRadarIcon_Implementation(bool bShowRadarIcon);


# pragma endregion

		
};
