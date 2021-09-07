// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "C_DeathHUDWidget.generated.h"

class UImage;
class UTextBlock;

UCLASS()
class HALOREACH_API UC_DeathHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UC_DeathHUDWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;

	// Default colour for all text
	FSlateColor HUDTextColour;

	void UpdateTextBlock(UTextBlock* TextBlock, FString NewText);

# pragma region PlayerKiller

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* PlayerKillerText;

# pragma endregion

# pragma region Respawn Data

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* RespawnTimerText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* RespawnLocationText;

# pragma endregion

# pragma region Black Fade In Animation

	void StoreWidgetAnimation();

	UWidgetAnimation* GetAnimationByName(FName AnimationName) const;

	TMap<FName, UWidgetAnimation*> AnimationsMap;

	UWidgetAnimation* FadeInAnimation;

	UWidgetAnimation* FadeOutAnimation;

	UFUNCTION()
	void PlayFadeInAnimation();

	UFUNCTION()
	void PlayFadeOutAnimation();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BindWidget))
	UImage* BlackImage;

# pragma endregion
	
};
