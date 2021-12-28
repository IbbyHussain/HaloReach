// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "C_PlayerHUDWidget.generated.h"

class UProgressBar;
class UImage;

UCLASS()
class HALOREACH_API UC_PlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UC_PlayerHUDWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UProgressBar* LeftShieldBar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UProgressBar* RightShieldBar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* HealthImg;

	void UpdateHealthImage(float Health);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Health")
	TArray<class UTexture2D*> HealthImages;

	// Shields

	void UpdateShieldBars(float Shields);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Health")
	UTexture2D* LeftShieldDefaultImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Health")
	UTexture2D* RightShieldDefaultImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* WarningText;

	UPROPERTY(EditAnywhere, Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
	UWidgetAnimation* WarningTextFadeAnimation;

	void SetHealthBarColour(FColor Color, FColor TintColor);

	UPROPERTY(EditAnywhere, Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
	UWidgetAnimation* HPBarFadeAnimation;

	void UpdateHPBarFadeAnimation(float HP);

	// Colours of the health bar, set in editor

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Colour")
	FColor Yellow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Colour")
	FColor Blue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Colour")
	FColor LightOrange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Colour")
	FColor Orange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Colour")
	FColor LightRed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Colour")
	FColor YellowTint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Colour")
	FColor NoTint;

	// Weapons

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* PrimaryWeaponImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* SecondaryWeaponImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* PrimaryMaxAmmoText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* SecondaryMaxAmmoText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* AmmoCounterImage;

	// Updates the primary and secondary weapon images
	UFUNCTION(BlueprintCallable)
	void UpdateWeaponImages();

	// Update the max ammo text for primary and secondary weapons
	UFUNCTION(BlueprintCallable)
	void UpdateAmmoText();

	UFUNCTION(BlueprintCallable)
	void UpdateAmmoImage();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UC_CrosshairWidget* Crosshair;

# pragma region Black Fade In Animation

	void StoreWidgetAnimation();

	UWidgetAnimation* GetAnimationByName(FName AnimationName) const;

	TMap<FName, UWidgetAnimation*> AnimationsMap;

	UWidgetAnimation* FadeInAnimation;

	UWidgetAnimation* FadeOutAnimation;

	UFUNCTION()
	void PlayFadeInAnimation();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BindWidget))
	UImage* BlackImage;

# pragma endregion

# pragma region Grenade Counter

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* PrimaryGrenadeCounter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* SecondaryGrenadeCounter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* PrimaryGrenadeImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* SecondaryGrenadeImage;

	UFUNCTION(BlueprintCallable)
	void UpdateGrenadeCounterText();

	UFUNCTION(BlueprintCallable)
	void UpdateGrenadeCounterImage();


# pragma endregion


# pragma region Radar

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UC_Radar* RadarWidget;

	// The radar works by rotating itslef based on the players world rotation
	UFUNCTION(BlueprintCallable)
	void UpdateRadarRotation();

# pragma endregion

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UC_CardinalDirectionsWidget* CardinalWidget;


};
