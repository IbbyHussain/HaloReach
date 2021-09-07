// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/UI/Names/C_GlobalAlertWidget.h"
#include "Components/TextBlock.h"

UC_GlobalAlertWidget::UC_GlobalAlertWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UC_GlobalAlertWidget::NativeConstruct()
{
}

void UC_GlobalAlertWidget::SetAlertText(const FString UpdatedAlertText)
{
	AlertText->SetText(FText::FromString(UpdatedAlertText));
}