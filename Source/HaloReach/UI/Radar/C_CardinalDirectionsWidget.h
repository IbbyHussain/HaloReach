// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HaloReach/GlobalStructs.h"
#include "C_CardinalDirectionsWidget.generated.h"

class UC_DirectionWidget;

UCLASS()
class HALOREACH_API UC_CardinalDirectionsWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UC_CardinalDirectionsWidget(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class USizeBox* MainBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UOverlay* MainOverlay;

	UFUNCTION(BlueprintCallable)
	void AddDirectionWidget(FDirectionsStruct DirectionsInfo);

	UC_DirectionWidget* DirectionWidget;

	UPROPERTY(EditDefaultsOnly, Category = "Cardinal Directions")
	TSubclassOf<UC_DirectionWidget> DirectionWidgetClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Cardinal Directions")
	TArray<UC_DirectionWidget*> DirectionWidgetArray;
};
