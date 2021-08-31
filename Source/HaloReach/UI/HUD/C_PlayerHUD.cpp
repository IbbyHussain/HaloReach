// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/UI/HUD/C_PlayerHUD.h"
#include "HaloReach/UI/HUD/C_PlayerHUDWidget.h"
#include "HaloReach/Player/PlayerExtra/C_ReachPlayerController.h"
#include "Engine/World.h"
#include "HaloReach/Player/C_PlayerCharacter.h"
#include "HaloReach/UI/HUD/C_DeathHUDWidget.h"
#include "HaloReach/UI/HUD/C_CrosshairWidget.h"
#include "HaloReach/UI/C_PlayerNameWidget.h"
#include "GameFramework/PlayerState.h"
#include "HaloReach/UI/C_NameInputWidget.h"
#include "Kismet/GameplayStatics.h"


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

	//CreatePlayerNameWidget();

	//CreateNameInputWidget();

	//CreateHUDWidget();

	//SetPlayerNameTextHUD(GetOwningPlayerController()->PlayerState->GetPlayerName());

}

void AC_PlayerHUD::CreateHUDWidget()
{
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

void AC_PlayerHUD::CreateDeathWidget()
{
	if (DeathWidgetClass)
	{
		DeathWidget = CreateWidget<UC_DeathHUDWidget>(GetWorld(), DeathWidgetClass);
		if (DeathWidget)
		{
			DeathWidget->AddToViewport();
		}
	}
}

void AC_PlayerHUD::DestroyDeathWidget()
{
	if(DeathWidget)
	{
		DeathWidget->RemoveFromParent();
	}
}

void AC_PlayerHUD::CreateNameInputWidget()
{
	if (NameInputWidgetClass)
	{
		NameInputWidget = CreateWidget<UC_NameInputWidget>(GetWorld(), NameInputWidgetClass);
		if (NameInputWidget)
		{
			NameInputWidget->AddToViewport();
		}
	}
}

void AC_PlayerHUD::DestroyNameInputWidget()
{
	if(NameInputWidget)
	{
		NameInputWidget->RemoveFromParent();
		CreateHUDWidget();
		CreatePlayerNameWidget();

		//APlayerController* PC = (UGameplayStatics::GetPlayerController(GetWorld(), 0));
		//AC_ReachPlayerController* RPC = Cast<AC_ReachPlayerController>(PC);

		//RPC->AssignedName = ("HUD ASSIGNED");
		//SetPlayerNameTextHUD(RPC->AssignedName);
	}
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

# pragma region PlayerName Widget

void AC_PlayerHUD::SetPlayerNameTextHUD(FString PlayerName)
{
	if(NameWidget)
	{
		NameWidget->SetPlayerNameText(PlayerName);
	}
}

void AC_PlayerHUD::CreatePlayerNameWidget()
{
	if (NameWidgetClass)
	{
		NameWidget = CreateWidget<UC_PlayerNameWidget>(GetWorld(), NameWidgetClass);
		if (NameWidget)
		{
			NameWidget->AddToViewport();
		}
	}
}

void AC_PlayerHUD::DestroyPlayerNameWidget()
{
	if (NameWidget)
	{
		NameWidget->RemoveFromParent();
	}
}

# pragma endregion

void AC_PlayerHUD::PlayHUDFadeInAnimation()
{
	if(DeathWidget)
	{
		DeathWidget->PlayFadeInAnimation();
	}
}

void AC_PlayerHUD::PlayHUDFadeOutAnimation()
{
	if (DeathWidget)
	{
		DeathWidget->PlayFadeOutAnimation();
	}
}

FString AC_PlayerHUD::HUDTestname()
{
	if(NameInputWidget)
	{
		return NameInputWidget->TestName();
	}

	return("FAIL");
}
