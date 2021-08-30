// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/UI/C_PlayerNameWidget.h"
#include "Components/TextBlock.h"

UC_PlayerNameWidget::UC_PlayerNameWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UC_PlayerNameWidget::NativeConstruct()
{
}

void UC_PlayerNameWidget::SetPlayerNameText(FString PlayerName)
{
	PlayerNameText->SetText(FText::FromString(PlayerName));
}
