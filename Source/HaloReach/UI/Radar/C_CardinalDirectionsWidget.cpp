// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/UI/Radar/C_CardinalDirectionsWidget.h"
#include "Components/SizeBox.h"
#include "Components/OverlaySlot.h"
#include "HaloReach/Libraries/C_SpawnLibrary.h"
#include "Components/Overlay.h"
#include "HaloReach/UI/Radar/C_DirectionWidget.h"

UC_CardinalDirectionsWidget::UC_CardinalDirectionsWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	//MainBox->SetHeightOverride(70.0f);
	//MainBox->SetWidthOverride(696.0f);
}

void UC_CardinalDirectionsWidget::AddDirectionWidget(FDirectionsStruct DirectionsInfo)
{

	DirectionWidget = UC_SpawnLibrary::SpawnWidgetNoViewport(GetWorld(), DirectionWidgetClass, DirectionWidget);

	// Sets the struct values so that struct is updated
	DirectionWidget->Directions.DirectionName = DirectionsInfo.DirectionName;
	DirectionWidget->Directions.WorldRotation = DirectionsInfo.WorldRotation;

	DirectionWidgetArray.Add(DirectionWidget);

	// overlay settings
	UOverlaySlot* DirectionSlot =  MainOverlay->AddChildToOverlay(DirectionWidget);
	DirectionSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
	DirectionSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Bottom);
	
}
