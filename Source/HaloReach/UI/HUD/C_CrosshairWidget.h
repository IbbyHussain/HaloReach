// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "C_CrosshairWidget.generated.h"

class UBorder;
class AC_PlayerCharacter;
class UImage;

UCLASS()
class HALOREACH_API UC_CrosshairWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UC_CrosshairWidget(const FObjectInitializer& ObjectInitializer);



#pragma region Borders

	// Inner Borders

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UBorder* top;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UBorder* bottom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UBorder* right;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UBorder* left;

	// Outter borders

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UBorder* topOuter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UBorder* bottomOuter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UBorder* rightOuter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UBorder* leftOuter;

# pragma endregion

# pragma region Crosshair Outer

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* CrosshairOuter;

	// Chnages crosshair texture depending on team affiliation of target
	UFUNCTION(BlueprintCallable)
	void SwitchCrosshairOuter(AC_PlayerCharacter* Target);

	UPROPERTY(EditAnywhere, Category = "Crosshair")
	int Team;

	UPROPERTY(EditAnywhere, Category = "Crosshair")
	TArray<UTexture2D*> CrosshairOuterArray;

# pragma endregion

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float crosshair_spread;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float crosshair_thickness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float crosshair_length;

	UFUNCTION(BlueprintCallable)
	float AddCrosshairSpread(float Amount, float DefaultSpread, float MaxSpread);
};
