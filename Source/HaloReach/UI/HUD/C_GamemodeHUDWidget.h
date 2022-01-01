// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "C_GamemodeHUDWidget.generated.h"

class UTextBlock;
class UImage;

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

	// other players / teams score
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* EnemyScore;

	// This players / teams score
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* PlayerScore;

# pragma endregion

	float MatchTime;

	FString GameModeName;

	// will get the enemy with the highest score's, score
	UFUNCTION(BlueprintCallable)
	int GetTopEnemyScore();

	UFUNCTION(BlueprintCallable)
	int GetPlayerScore();

# pragma region Player Score Animation

	void StoreWidgetAnimation();

	UWidgetAnimation* GetAnimationByName(FName AnimationName) const;

	TMap<FName, UWidgetAnimation*> AnimationsMap;

	UWidgetAnimation* MovePlayerScoreAnimation;

	UWidgetAnimation* MoveEnemyScoreAnimation;

	UFUNCTION(BlueprintCallable)
	void SwapScoreElements();

	UFUNCTION(BlueprintCallable)
	void LostLead();

	UFUNCTION(BlueprintCallable)
	void GainedLead();

	bool bSwap = true;

	int HighestEnemyScore;

#pragma endregion 
};
