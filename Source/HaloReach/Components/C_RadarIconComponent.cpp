// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/Components/C_RadarIconComponent.h"
#include "Kismet/GameplayStatics.h"
#include "HaloReach/UI/HUD/C_PlayerHUD.h"
#include "HaloReach/UI/HUD/C_PlayerHUDWidget.h"
#include "HaloReach/UI/Radar/C_Radar.h"


UC_RadarIconComponent::UC_RadarIconComponent()
{

}

void UC_RadarIconComponent::BeginPlay()
{
	Super::BeginPlay();

	HUD = Cast<AC_PlayerHUD>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD());
}

void UC_RadarIconComponent::AddRadarIcon()
{
	if(GetOwner())
	{
		if(HUD)
		{
			// Will add this components radar icon to the players radar widget
			if(HUD->HUDWidget)
			{
				RadarIcon = HUD->HUDWidget->RadarWidget->AddPOI(GetOwner());
				SetRadar();
				
				
			}
		}
	}
}

