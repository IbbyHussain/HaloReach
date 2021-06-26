// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/UI/HUD/C_PlayerHUD.h"
#include "HaloReach/UI/HUD/C_PlayerHUDWidget.h"
#include "Engine/World.h"

AC_PlayerHUD::AC_PlayerHUD()
{

}

void AC_PlayerHUD::DrawHUD()
{
	Super::DrawHUD();
}

void AC_PlayerHUD::AddWidgetToViewport(UUserWidget* Widget, TSubclassOf<UUserWidget> WidgetClass)
{
	if (WidgetClass)
	{
		Widget = CreateWidget<UUserWidget>(GetWorld(),  WidgetClass);
		if (Widget)
		{
			Widget->AddToViewport();
		}
	}
}

void AC_PlayerHUD::BeginPlay()
{
	Super::BeginPlay();


	//AddWidgetToViewport(HUDWidget, HUDWidgetClass);

	if (HUDWidgetClass)
	{
		HUDWidget = CreateWidget<UC_PlayerHUDWidget>(GetWorld(), HUDWidgetClass);
		if (HUDWidget)
		{
			HUDWidget->AddToViewport();
		}
	}

}

void AC_PlayerHUD::HUDUpdateHealthImage(float Health)
{
	if(HUDWidget)
	{
		HUDWidget->UpdateHealthImage(Health);
		//UE_LOG(LogTemp, Error, TEXT("HUD CALLED"));
	}
}

void AC_PlayerHUD::HUDUpdateShieldBars(float Shields)
{
	if (HUDWidget)
	{
		HUDWidget->UpdateShieldBars(Shields);
	}
}

