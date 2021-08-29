// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "C_GamemodeHUDWidget.generated.h"

class UTextBlock;

UCLASS()
class HALOREACH_API UC_GamemodeHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	// Default colour for all text
	FSlateColor HUDTextColour;

	UC_GamemodeHUDWidget(const FObjectInitializer& ObjectInitializer);
	
	virtual void NativeConstruct() override;

#pragma region Gamemode Data

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* MatchTimerText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* GamemodeNameText;

# pragma endregion


};
