// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/UI/Radar/C_DirectionWidget.h"
#include "Components/TextBlock.h"

UC_DirectionWidget::UC_DirectionWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UC_DirectionWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	// Changes the direction name to only two letters, then make sthem uppercase
	FString DirectionString = Directions.DirectionName.ToString();
	FString FinalString =  DirectionString.LeftChop(DirectionString.Len() - 2).ToUpper();

	DirectionName->SetText(FText::FromString(FinalString));
}