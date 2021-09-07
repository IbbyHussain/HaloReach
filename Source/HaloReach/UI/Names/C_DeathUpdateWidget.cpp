// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/UI/Names/C_DeathUpdateWidget.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"

UC_DeathUpdateWidget::UC_DeathUpdateWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UC_DeathUpdateWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UC_DeathUpdateWidget::UpdateAlertBox(UWidget* AlertWidget)
{
	AlertBox->AddChildToVerticalBox(AlertWidget);
}
