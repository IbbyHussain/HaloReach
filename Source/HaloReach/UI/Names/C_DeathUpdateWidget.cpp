// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/UI/Names/C_DeathUpdateWidget.h"
#include "Components/TextBlock.h"

UC_DeathUpdateWidget::UC_DeathUpdateWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UC_DeathUpdateWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//UpdateDeathText("begin");
}

void UC_DeathUpdateWidget::UpdateDeathText(FString NewText)
{
	DeathUpdateText->SetText(FText::FromString(NewText));
}