// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "C_PlayerHUD.generated.h"

UCLASS()
class HALOREACH_API AC_PlayerHUD : public AHUD
{
	GENERATED_BODY()

public:
	AC_PlayerHUD();

	virtual void DrawHUD() override;

	virtual void BeginPlay() override;

	void AddWidgetToViewport(UUserWidget* Widget, TSubclassOf<UUserWidget> WidgetClass);

	// HUD Widget

	class UC_PlayerHUDWidget* HUDWidget;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<class UC_PlayerHUDWidget> HUDWidgetClass;

	// Health System

	UFUNCTION(BlueprintCallable)
	void HUDUpdateHealthImage(float Health);

	UFUNCTION(BlueprintCallable)
	void HUDUpdateShieldBars(float Shields);

	// Ammo counter

	UFUNCTION()
	void UpdateWeaponReserves();
};
