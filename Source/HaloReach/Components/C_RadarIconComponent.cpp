// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/Components/C_RadarIconComponent.h"
#include "Kismet/GameplayStatics.h"
#include "HaloReach/UI/HUD/C_PlayerHUD.h"
#include "HaloReach/UI/HUD/C_PlayerHUDWidget.h"
#include "HaloReach/UI/Radar/C_Radar.h"
#include "HaloReach/UI/Radar/C_RadarIcon.h"
#include "HaloReach/Player/PlayerExtra/C_ReachPlayerController.h"
#include "HaloReach/Player/C_PlayerCharacter.h"
#include "EngineUtils.h"

//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Added icon")));

UC_RadarIconComponent::UC_RadarIconComponent()
{

}

void UC_RadarIconComponent::BeginPlay()
{
	Super::BeginPlay();

	HUD = Cast<AC_PlayerHUD>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD());

	// Adds the owners radar icon
	if(!RadarIcon)
	{
		AddRadarIcon();
	}
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
			}
		}
	}
}

void UC_RadarIconComponent::RemoveRadarIcon()
{
	if(GetOwner()->HasAuthority())
	{
		Multi_RemoveRadarIcon();
	}

	else
	{
		Server_RemoveRadarIcon();
	}
}

void UC_RadarIconComponent::Server_RemoveRadarIcon_Implementation()
{
	Multi_RemoveRadarIcon();
}

void UC_RadarIconComponent::Multi_RemoveRadarIcon_Implementation()
{
	RadarIcon->RemoveFromParent();

	AC_ReachPlayerController* RPC = Cast<AC_ReachPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if(RPC)
	{
		RPC->bHasRespawned = true;
	}
}

void UC_RadarIconComponent::RefreshRadarIcons()
{
	AC_ReachPlayerController* RPC = Cast<AC_ReachPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (RPC)
	{
		if(RPC->bHasRespawned)
		{
			// Use iterator to get all valid player characters
			AC_PlayerCharacter* PlayerCharacter;
			TArray<AC_PlayerCharacter*> CurrentPlayerArray;

			for (TActorIterator<AC_PlayerCharacter> It(GetWorld(), AC_PlayerCharacter::StaticClass()); It; ++It)
			{
				PlayerCharacter = *It;
				if (PlayerCharacter != NULL)
				{
					CurrentPlayerArray.Add(PlayerCharacter);
					
					// Get valid players radar icon and add it to this players radar
					PlayerCharacter->GetRadarComponent()->AddRadarIcon();

					// Ensures this method is only called once
					RPC->bHasRespawned = false;
				}
			}

		}
	}
}

# pragma region Toggle Radar Icon

void UC_RadarIconComponent::ToggleRadarIcon(bool bShowRadarIcon)
{
	if (GetOwner()->HasAuthority())
	{
		Multi_ToggleRadarIcon(bShowRadarIcon);
	}

	else
	{
		Server_ToggleRadarIcon(bShowRadarIcon);
	}
	
}

void UC_RadarIconComponent::Server_ToggleRadarIcon_Implementation(bool bShowRadarIcon)
{
	Multi_ToggleRadarIcon(bShowRadarIcon);
}

void UC_RadarIconComponent::Multi_ToggleRadarIcon_Implementation(bool bShowRadarIcon)
{
	AC_PlayerCharacter* PlayerCharacter = Cast<AC_PlayerCharacter>(GetOwner());

	// Ensures that the radar icon's visibility is not changed on local player but changes for everyone else
	if (PlayerCharacter && !PlayerCharacter->IsLocallyControlled())
	{
		bShowRadarIcon ? RadarIcon->SetVisibility(ESlateVisibility::Visible) : RadarIcon->SetVisibility(ESlateVisibility::Hidden);
	}
}

# pragma endregion