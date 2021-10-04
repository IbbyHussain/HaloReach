// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "C_PlayerHUD.generated.h"

class UC_DeathHUDWidget;
class UC_PlayerNameWidget;
class UC_NameInputWidget;
class UC_DeathUpdateWidget;
class UC_GlobalAlertWidget;

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

	void CreateHUDWidget();

	void HideHUDWidget();

	void ShowHUDWidget();

	void PlaySpawnFadeIn();

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

# pragma region PlayerName Widget

	void SetPlayerNameTextHUD(FString PlayerName);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UC_PlayerNameWidget> NameWidgetClass;

	UPROPERTY()
	UC_PlayerNameWidget* NameWidget;

	void CreatePlayerNameWidget();

	void DestroyPlayerNameWidget();

# pragma endregion
	
# pragma region DeathWidget

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UC_DeathHUDWidget> DeathWidgetClass;

	UPROPERTY()
	UC_DeathHUDWidget* DeathWidget;

	void CreateDeathWidget();

	void DestroyDeathWidget();

# pragma endregion

# pragma region Player Input Name Widget

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UC_NameInputWidget> NameInputWidgetClass;

	UPROPERTY()
	UC_NameInputWidget* NameInputWidget;

	void CreateNameInputWidget();

	void DestroyNameInputWidget();

# pragma endregion

# pragma region Global Death Updates

	void CreateDeathUpdateWidget();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UC_DeathUpdateWidget> DeathUpdateWidgetClass;
		
	UPROPERTY()
	UC_DeathUpdateWidget* DeathUpdateWidget;

	UFUNCTION(BlueprintCallable)
	void UpdateDeathWidget(FString A, FString B);

# pragma endregion
	
# pragma region Widget Animation

	void PlayHUDFadeInAnimation();

	void PlayHUDFadeOutAnimation();

# pragma endregion

public:

	UFUNCTION(BlueprintCallable)
	FString HUDTestname();

	UPROPERTY(BlueprintReadWrite)
	FString DeathText;

};
