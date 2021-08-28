// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/UI/HUD/C_PlayerHUD.h"
#include "HaloReach/UI/HUD/C_PlayerHUDWidget.h"
#include "Engine/World.h"
#include "HaloReach/Player/C_PlayerCharacter.h"
#include "HaloReach/UI/HUD/C_CrosshairWidget.h"

AC_PlayerHUD::AC_PlayerHUD()
{

}

void AC_PlayerHUD::DrawHUD()
{
	Super::DrawHUD();
}

void AC_PlayerHUD::AddWidgetToViewport(UUserWidget*& Widget, TSubclassOf<UUserWidget> WidgetClass)
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

void AC_PlayerHUD::DestroyWidget(UUserWidget*& Widget)
{
	if (Widget)
	{
		Widget->RemoveFromParent();
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

void AC_PlayerHUD::HideHUDWidget()
{
	if(HUDWidget)
	{
		HUDWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AC_PlayerHUD::ShowHUDWidget()
{
	if(HUDWidget)
	{
		HUDWidget->SetVisibility(ESlateVisibility::Visible);
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

void AC_PlayerHUD::UpdateWeaponReserves()
{
	if(HUDWidget)
	{
		HUDWidget->UpdateAmmoText();
		DestroyWidget(ZoomWidget);
	}
}

void AC_PlayerHUD::CreateZoomWidget(TSubclassOf<UUserWidget> WidgetClass)
{
	AddWidgetToViewport(ZoomWidget, WidgetClass);
}

void AC_PlayerHUD::DestroyZoomWidget()
{
	DestroyWidget(ZoomWidget);
}

// crosshair

float AC_PlayerHUD::AddCrosshairSpreadHUD(float Amount, float DefaultSpread, float MaxSpread)
{
	if(HUDWidget)
	{
		return HUDWidget->Crosshair->AddCrosshairSpread(Amount, DefaultSpread, MaxSpread);
	}

	return 0.0f;
}

void AC_PlayerHUD::PlayHUDFadeInAnimation()
{
	if(HUDWidget)
	{
		HUDWidget->PlayFadeInAnimation();
	}
}

void AC_PlayerHUD::PlayHUDFadeOutAnimation()
{
	if (HUDWidget)
	{
		HUDWidget->PlayFadeOutAnimation();
	}
}
