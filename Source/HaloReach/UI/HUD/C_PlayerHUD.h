// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "C_PlayerHUD.generated.h"

class UC_DeathHUDWidget;

UCLASS()
class HALOREACH_API AC_PlayerHUD : public AHUD
{
	GENERATED_BODY()

public:
	AC_PlayerHUD();

	virtual void DrawHUD() override;

	virtual void BeginPlay() override;

	void AddWidgetToViewport(UUserWidget*& Widget, TSubclassOf<UUserWidget> WidgetClass);

	void DestroyWidget(UUserWidget*& Widget);

	// HUD Widget
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Widgets")
	class UC_PlayerHUDWidget* HUDWidget;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<class UC_PlayerHUDWidget> HUDWidgetClass;

	void HideHUDWidget();

	void ShowHUDWidget();

	// Health System

	UFUNCTION(BlueprintCallable)
	void HUDUpdateHealthImage(float Health);

	UFUNCTION(BlueprintCallable)
	void HUDUpdateShieldBars(float Shields);

	// Ammo counter

	UFUNCTION()
	void UpdateWeaponReserves();

	// Zoom Scope

	UPROPERTY()
	UUserWidget* ZoomWidget;

	void CreateZoomWidget(TSubclassOf<UUserWidget> WidgetClass);

	void DestroyZoomWidget();

public:

	UFUNCTION(BlueprintCallable)
	float AddCrosshairSpreadHUD(float Amount, float DefaultSpread, float MaxSpread);

	
# pragma region DeathWidget

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UC_DeathHUDWidget> DeathWidgetClass;

	UPROPERTY()
	UC_DeathHUDWidget* DeathWidget;

	void CreateDeathWidget();

	void DestroyDeathWidget();

# pragma endregion
	
# pragma region Widget Animation

	void PlayHUDFadeInAnimation();

	void PlayHUDFadeOutAnimation();

# pragma endregion

};
