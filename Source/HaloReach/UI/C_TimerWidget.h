// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "C_TimerWidget.generated.h"

class UTextBlock;

UCLASS()
class HALOREACH_API UC_TimerWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UC_TimerWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;

	// Default colour for all text
	FSlateColor HUDTextColour;

	// How many minutes the timer should last for
	UPROPERTY(EditAnywhere, Category = "Timer Stats")
	int MaxMinutes;

	// How many seconds the timer should last for
	UPROPERTY(EditAnywhere, Category = "Timer Stats")
	int MaxSeconds;

	// Current minutes
	int Minutes;

	// Current seconds
	int Seconds;

	FTimerHandle TimerHandle;

	void StartTimer();

	void UpdateTime();

	void UpdateSeconds();

	void UpdateMinutes();

	// Minutes and seconds are two separate text blocks as some timers will only need seconds

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* MinutesTimerText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* SecondsTimerText;
	
};
