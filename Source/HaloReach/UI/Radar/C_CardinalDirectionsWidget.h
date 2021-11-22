// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "C_CardinalDirectionsWidget.generated.h"

class USizeBox;
class UImage;

UCLASS()
class HALOREACH_API UC_CardinalDirectionsWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UC_CardinalDirectionsWidget(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	USizeBox* MainBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* DirectionsImage;


	
};
