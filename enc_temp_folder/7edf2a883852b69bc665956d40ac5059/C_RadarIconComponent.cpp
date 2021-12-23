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
#include "TimerManager.h"
#include <Runtime/Engine/Public/Net/UnrealNetwork.h>
#include "HaloReach/Player/C_PlayerCharacter.h"
#include "HaloReach/Components/C_TeamsComponent.h"
#include "Kismet/GameplayStatics.h"

//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Added icon")));

UC_RadarIconComponent::UC_RadarIconComponent()
{

}

void UC_RadarIconComponent::BeginPlay()
{
	Super::BeginPlay();

	HUD = Cast<AC_PlayerHUD>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD());

	PlayerOwner = Cast<AC_PlayerCharacter>(GetOwner());

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

# pragma region Show Radar Icon

// Shows radar icon (excludes owning client)
void UC_RadarIconComponent::ShowRadarIcon(bool bAutoHide)
{
	if (GetOwner()->HasAuthority())
	{
		Multi_ShowRadarIcon();
	}

	else
	{
		Server_ShowRadarIcon();
	}

	// When the current action will not manually call HideRadarIcon, Used for melee and firing weapon etc
	if (bAutoHide)
	{
		// Sets render opacity back to 0 after delay
		RadarIconFadeDelegate.BindUFunction(this, FName("HideRadarIcon"), true);
		GetWorld()->GetTimerManager().SetTimer(RadarIconFadeHandle, RadarIconFadeDelegate, 1.0f, false);
	}
}

void UC_RadarIconComponent::Server_ShowRadarIcon_Implementation()
{
	Multi_ShowRadarIcon();
}

void UC_RadarIconComponent::Multi_ShowRadarIcon_Implementation()
{
	AC_PlayerCharacter* PlayerCharacter = Cast<AC_PlayerCharacter>(GetOwner());

	// Ensures that the radar icon's visibility is not changed on local player but changes for everyone else
	if (PlayerCharacter && !PlayerCharacter->IsLocallyControlled() && RadarIcon)
	{
		RadarIcon->StopFadeOutAnimation();
		RadarIcon->SetRenderOpacity(1.0f);
	}
}

# pragma endregion

# pragma region Hide Radar Icon

// Hides radar icon (excludes owning client)
void UC_RadarIconComponent::HideRadarIcon(bool bPlayFadeAnim)
{
	if (GetOwner()->HasAuthority())
	{
		Multi_HideRadarIcon(bPlayFadeAnim);
	}

	else
	{
		Server_HideRadarIcon(bPlayFadeAnim);
	}
}

void UC_RadarIconComponent::Server_HideRadarIcon_Implementation(bool bPlayFadeAnim)
{
	Multi_HideRadarIcon(bPlayFadeAnim);
}

void UC_RadarIconComponent::Multi_HideRadarIcon_Implementation(bool bPlayFadeAnim)
{
	AC_PlayerCharacter* PlayerCharacter = Cast<AC_PlayerCharacter>(GetOwner());

	// Ensures that the radar icon's visibility is not changed on local player but changes for everyone else
	if (PlayerCharacter && !PlayerCharacter->IsLocallyControlled() && RadarIcon)
	{
		bPlayFadeAnim ? RadarIcon->PlayFadeOutAnimation() : RadarIcon->SetRenderOpacity(0.0f);
	}
}

# pragma endregion

// If the player is walking, will need to clear the fade out handle as walking uses its own calls for fading out radar icon
void UC_RadarIconComponent::ClearRadarIconFadeHandle()
{
	if(GetWorld()->GetTimerManager().IsTimerActive(RadarIconFadeHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(RadarIconFadeHandle);
	}
}



# pragma endregion

void UC_RadarIconComponent::ShowRadarIcon2(bool bHide)
{
	RadarIcon->StopFadeOutAnimation();
	RadarIcon->SetRenderOpacity(1.0f);

	// When the current action will not manually call HideRadarIcon, Used for melee and firing weapon etc
	if (bHide)
	{
		// Sets render opacity back to 0 after delay
		RadarIconFadeDelegate.BindUFunction(this, FName("HideRadarIcon2"), true);
		GetWorld()->GetTimerManager().SetTimer(RadarIconFadeHandle, RadarIconFadeDelegate, 1.0f, false);
	}
}

void UC_RadarIconComponent::HideRadarIcon2(bool bPlayFade)
{
	if(RadarIcon)
	{
		bPlayFade ? RadarIcon->PlayFadeOutAnimation() : RadarIcon->SetRenderOpacity(0.0f);
	}
	
	
}

void UC_RadarIconComponent::Server_UpdateRadarIcon_Implementation()
{

}

void UC_RadarIconComponent::Client_HideRadarIcon_Implementation(AC_PlayerCharacter* ClientPlayerOwner)
{
	ClientPlayerOwner->GetRadarComponent()->ShowRadarIcon2(true);
}

# pragma region Update Radar Icons on Team Change

void UC_RadarIconComponent::Server_UpdateAllPlayersRadarIcons_Implementation()
{
	for(auto i : GetWorld()->GetGameState()->PlayerArray)
	{
		i->GetPawn<AC_PlayerCharacter>()->GetRadarComponent()->Server_ComapreTeams();
	}
}

void UC_RadarIconComponent::Server_ComapreTeams_Implementation()
{
	for (auto i : GetWorld()->GetGameState()->PlayerArray)
	{
		if(PlayerOwner->GetTeamsComponent()->GetTeam() == i->GetPawn<AC_PlayerCharacter>()->GetTeamsComponent()->GetTeam())
		{
			Client_SetRadarIconOpacity(true, i->GetPawn<AC_PlayerCharacter>());
		}

		else
		{
			Client_SetRadarIconOpacity(false, i->GetPawn<AC_PlayerCharacter>());
		}
		
	}
}

void UC_RadarIconComponent::Client_SetRadarIconOpacity_Implementation(bool bSameTeam, AC_PlayerCharacter* PlayerPTR)
{
	if(bSameTeam)
	{
		PlayerPTR->GetRadarComponent()->RadarIconImage = TAllyIcon;
		PlayerPTR->GetRadarComponent()->RadarIcon->SetRenderOpacity(1.0f);
	}

	else
	{
		PlayerPTR->GetRadarComponent()->RadarIconImage = TEnemyIcon;
		PlayerPTR->GetRadarComponent()->RadarIcon->SetRenderOpacity(0.0f);
	}
}

# pragma endregion