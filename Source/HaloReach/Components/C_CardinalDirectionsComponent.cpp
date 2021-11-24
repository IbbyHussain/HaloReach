
#include "HaloReach/Components/C_CardinalDirectionsComponent.h"
#include "Kismet/GameplayStatics.h"
#include "HaloReach/UI/HUD/C_PlayerHUD.h"
#include "HaloReach/UI/HUD/C_PlayerHUDWidget.h"
#include "HaloReach/Player/C_PlayerCharacter.h"
#include "GenericPlatform/GenericPlatformMath.h"
#include "HaloReach/UI/Radar/C_CardinalDirectionsWidget.h"
#include "HaloReach/UI/Radar/C_DirectionWidget.h"

UC_CardinalDirectionsComponent::UC_CardinalDirectionsComponent()
{

	PrimaryComponentTick.bCanEverTick = true;

	
	
}


void UC_CardinalDirectionsComponent::BeginPlay()
{
	Super::BeginPlay();
}



void UC_CardinalDirectionsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

float UC_CardinalDirectionsComponent::RotationToCirlceDegrees(float Rotation)
{
	if(Rotation < 0.0f)
	{
		return FGenericPlatformMath::Fmod(Rotation + 360.0f, 360.0f);
	}

	return Rotation;
}

float UC_CardinalDirectionsComponent::GetDeltaClockwise(float A, float B, bool bIsClockwise)
{
	float MA = FGenericPlatformMath::Fmod(A, 360.0f);
	float MB = FGenericPlatformMath::Fmod(B, 360.0f);

	if(bIsClockwise)
	{
		if(MA > MB)
		{
			return 360.0f - (MA - MB);
		}

		else if(MA == MB)
		{
			return 0.0f;
		}

		else if(MA < MB)
		{
			return MB - MA;
		}
	}

	else
	{
		if (MA > MB)
		{
			return MA - MB;
		}

		else if (MA == MB)
		{
			return 0.0f;
		}

		else if (MA < MB)
		{
			return 360.0f - (MB - MA);
		}
	}

	return 0.0f;
}

FTranslationsOutput UC_CardinalDirectionsComponent::RotationToTranslation(FRotator A, FRotator B)
{
	float AYaw = A.Yaw;
	float BYaw = B.Yaw;

	// A stuff
	float AYawDegrees = RotationToCirlceDegrees(AYaw);
	float BYawDegrees = RotationToCirlceDegrees(BYaw);

	float ClockwiseDelta = GetDeltaClockwise(AYawDegrees, BYawDegrees, true);
	float AntiClockwiseDelta = GetDeltaClockwise(AYawDegrees, BYawDegrees, false);

	if(ClockwiseDelta > AntiClockwiseDelta)
	{
		if( (AntiClockwiseDelta / PlayerCameraComp->FieldOfView) <= 1.0f)
		{
			float XValue = (FMath::Lerp(0.0f, MaxWidgetTranslation, AntiClockwiseDelta / PlayerCameraComp->FieldOfView)) * -1.0f;
			return {FVector2D(XValue, 0.0f), false, true};
		}

		return{FVector2D(0.0f), false, false};
	}

	else if(ClockwiseDelta == AntiClockwiseDelta || ClockwiseDelta < AntiClockwiseDelta)
	{
		if((ClockwiseDelta / PlayerCameraComp->FieldOfView) <= 1.0f)
		{
			float XXValue = FMath::Lerp(0.0f, MaxWidgetTranslation, ClockwiseDelta / PlayerCameraComp->FieldOfView);
			return{ FVector2D(XXValue, 0.0f), true, true };
		}

		return{ FVector2D(0.0f), true, false };
	}

	return { FVector2D(0.0f), false, false };
}

void UC_CardinalDirectionsComponent::UpdateDirectionWidget()
{
	if(CardinalWidgetptr)
	{
		for (int i = 0; i < CardinalWidgetptr->DirectionWidgetArray.Num(); i++)
		{
			FTranslationsOutput TranslationOutput = RotationToTranslation(PlayerCameraComp->GetComponentRotation(), FRotator(0.0f, DirectionsArray[i].WorldRotation, 0.0f));

			if (TranslationOutput.bInRadarSight)
			{
				CardinalWidgetptr->DirectionWidgetArray[i]->SetRenderTranslation(TranslationOutput.Translation);
				if (!CardinalWidgetptr->DirectionWidgetArray[i]->IsVisible())
				{
					CardinalWidgetptr->DirectionWidgetArray[i]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				}

			}

			else
			{
				if (CardinalWidgetptr->DirectionWidgetArray[i]->IsVisible())
				{
					CardinalWidgetptr->DirectionWidgetArray[i]->SetVisibility(ESlateVisibility::Hidden);
				}
			}
		}
	}
	
}

void UC_CardinalDirectionsComponent::InitDirectionsComponent(UCameraComponent* PlayerCamera)
{
	PlayerCameraComp = PlayerCamera;

	AC_PlayerHUD* HUD = Cast<AC_PlayerHUD>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD());
	if(HUD)
	{
		CardinalWidgetptr = HUD->HUDWidget->CardinalWidget;
		if(CardinalWidgetptr)
		{
			for (int i = 0; i < DirectionsArray.Num(); i++)
			{
				CardinalWidgetptr->AddDirectionWidget(DirectionsArray[i]);
			}

			UpdateDirectionWidget();
		}
	}
}
