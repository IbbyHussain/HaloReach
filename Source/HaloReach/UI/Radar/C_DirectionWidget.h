// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HaloReach/GlobalEnums.h"
#include "HaloReach/GlobalStructs.h"
#include "C_DirectionWidget.generated.h"

class UHorizontalBox;
class UImage;
class UTextBlock;

UCLASS()
class HALOREACH_API UC_DirectionWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UC_DirectionWidget(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	FDirectionsStruct Directions;

	virtual void NativePreConstruct() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UHorizontalBox* MainBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* LeftImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* RightImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* DirectionName;
};
