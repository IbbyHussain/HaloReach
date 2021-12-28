// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "C_GamemodeHUDWidget.generated.h"

/*USTRUCT()
struct FGameMode
{
	GENERATED_BODY()
public:

	float MatchTime;

	FString GameModeName;

	int 

	FGameMode()
	{
		
	}
};*/

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

	//FGameMode GameMode;

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
};
