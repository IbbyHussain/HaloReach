// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "C_RadarIconComponent.generated.h"

class AC_PlayerCharacter;

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

	// Sets Render opacity of radar icon to 1.0 ignores local player.
	UFUNCTION(BlueprintCallable)
	void ShowRadarIcon(bool bAutoHide);

	UFUNCTION(Server, Reliable)
	void Server_ShowRadarIcon();
	void Server_ShowRadarIcon_Implementation();

	UFUNCTION(NetMulticast, Reliable)
	void Multi_ShowRadarIcon();
	void Multi_ShowRadarIcon_Implementation();

	// Sets Render opacity of radar icon to 0.0 ignores local player. Boolean so that at start match other players are hidden striaght away
	UFUNCTION(BlueprintCallable)
	void HideRadarIcon(bool bPlayFadeAnim);

	UFUNCTION(Server, Reliable)
	void Server_HideRadarIcon(bool bPlayFadeAnim);
	void Server_HideRadarIcon_Implementation(bool bPlayFadeAnim);

	UFUNCTION(NetMulticast, Reliable)
	void Multi_HideRadarIcon(bool bPlayFadeAnim);
	void Multi_HideRadarIcon_Implementation(bool bPlayFadeAnim);

	// Will stop the fade out if the icon still needs to be displayed e.g when walking
	UFUNCTION(BlueprintCallable)
	void ClearRadarIconFadeHandle();

	// Plays fade out after a short delay
	FTimerHandle RadarIconFadeHandle;

	FTimerDelegate RadarIconFadeDelegate;

# pragma endregion

# pragma region Radar Toggle NEW

	UFUNCTION(BlueprintCallable)
	void ShowRadarIcon2(bool bHide);

	UFUNCTION(BlueprintCallable)
	void HideRadarIcon2(bool bPlayFade);

	UFUNCTION(Server, Reliable)
	void Server_UpdateRadarIcon();
	void Server_UpdateRadarIcon_Implementation();


	UFUNCTION(Client, BlueprintCallable, Reliable)
	void Client_HideRadarIcon(AC_PlayerCharacter* PlayerOwner);
	void Client_HideRadarIcon_Implementation(AC_PlayerCharacter* PlayerOwner);

	FTimerHandle Handle;

	FTimerDelegate Delegate;


# pragma endregion
		
};
