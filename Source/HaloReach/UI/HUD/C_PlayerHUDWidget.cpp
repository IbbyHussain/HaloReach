// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/UI/HUD/C_PlayerHUDWidget.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Styling/SlateBrush.h"
#include "Brushes/SlateImageBrush.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/TextBlock.h"
#include "Net/UnrealNetwork.h"

#include "Kismet/GameplayStatics.h"
#include "HaloReach/Player/C_PlayerCharacter.h"
#include "HaloReach/Interactables/Weapons/C_BaseWeapon.h"
#include "HaloReach/Interactables/Weapons/Guns/C_BaseGun.h"

UC_PlayerHUDWidget::UC_PlayerHUDWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	//PlayerCharacter* Character = Cast<PlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
}

void UC_PlayerHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	WarningText->SetText(FText::FromString("WARNING"));
	WarningText->SetVisibility(ESlateVisibility::Hidden);

	FLinearColor Colour = FColor(168, 235, 243, 100);
	LeftShieldBar->SetFillColorAndOpacity(Colour);
	RightShieldBar->SetFillColorAndOpacity(Colour);
}

void UC_PlayerHUDWidget::UpdateHealthImage(float Health)
{
	//UE_LOG(LogTemp, Log, TEXT("Widget called"));

	if((Health <= 45.0f) && (Health > 40.0f))
	{
		//UE_LOG(LogTemp, Log, TEXT("Health is between 40 and 45"));
		HealthImg->SetBrushFromTexture(HealthImages[9]);
		SetHealthBarColour(Blue, NoTint);
	}

	else if ((Health <= 40.0f) && (Health > 35.0f))
	{
		//UE_LOG(LogTemp, Log, TEXT("Health is between 35 and 40"));
		HealthImg->SetBrushFromTexture(HealthImages[8]);
		SetHealthBarColour(Yellow, YellowTint);
	}

	else if ((Health <= 35.0f) && (Health > 30.0f))
	{
		//UE_LOG(LogTemp, Log, TEXT("Health is between 30 and 35"));
		HealthImg->SetBrushFromTexture(HealthImages[7]);
		SetHealthBarColour(LightOrange, NoTint);
	}

	else if ((Health <= 30.0f) && (Health > 25.0f))
	{
		//UE_LOG(LogTemp, Log, TEXT("Health is between 25 and 30"));
		HealthImg->SetBrushFromTexture(HealthImages[6]);
		SetHealthBarColour(Orange, NoTint);
	}

	else if ((Health <= 25.0f) && (Health > 20.0f))
	{
		//UE_LOG(LogTemp, Log, TEXT("Health is between 20 and 25"));
		HealthImg->SetBrushFromTexture(HealthImages[5]);
		SetHealthBarColour(LightRed, NoTint);
	}

	else if ((Health <= 20.0f) && (Health > 15.0f))
	{
		//UE_LOG(LogTemp, Log, TEXT("Health is between 15 and 20"));

		HealthImg->SetBrushFromTexture(HealthImages[4]);
		SetHealthBarColour(LightRed, NoTint);
	}

	else if ((Health <= 15.0f) && (Health > 10.0f))
	{
		//UE_LOG(LogTemp, Log, TEXT("Health is between 10 and 15"));
		HealthImg->SetBrushFromTexture(HealthImages[3]);
		SetHealthBarColour(LightRed, NoTint);
	}

	else if ((Health <= 10.0f) && (Health > 5.0f))
	{
		//UE_LOG(LogTemp, Log, TEXT("Health is between 5 and 10"));
		HealthImg->SetBrushFromTexture(HealthImages[2]);
		SetHealthBarColour(LightRed, NoTint);
	}

	else if ((Health <= 5.0f) && (Health > 0.0f))
	{
		//UE_LOG(LogTemp, Log, TEXT("Health is between 0 and 5"));
		HealthImg->SetBrushFromTexture(HealthImages[1]);
		SetHealthBarColour(LightRed, NoTint);
	}

	else if (Health <= 0.0f)
	{
		//UE_LOG(LogTemp, Log, TEXT("Health is 0 "));
		HealthImg->SetBrushFromTexture(HealthImages[0]);
		SetHealthBarColour(LightRed, NoTint);
	}

	UpdateHPBarFadeAnimation(Health);
}

void UC_PlayerHUDWidget::UpdateHPBarFadeAnimation(float HP)
{
	if(HP <= 35.0f)
	{
		PlayAnimation(HPBarFadeAnimation, 0.0f, 0, EUMGSequencePlayMode::Forward, 1.0f);
	}

	else
	{
		StopAnimation(HPBarFadeAnimation);
	}
}

void UC_PlayerHUDWidget::SetHealthBarColour(FColor Color, FColor TintColor)
{
	HealthImg->SetColorAndOpacity(FLinearColor(Color));
	HealthImg->SetBrushTintColor(FSlateColor(TintColor));
}

void UC_PlayerHUDWidget::UpdateShieldBars(float Shields)
{
	FLinearColor Red = FLinearColor(1, .1, .1, 1);
	FSlateImageBrush LeftWarningBrush = FSlateImageBrush(LeftShieldDefaultImage, FVector2D(0, 0), FSlateColor(Red));
	FSlateImageBrush RightWarningBrush = FSlateImageBrush(RightShieldDefaultImage, FVector2D(0, 0), FSlateColor(Red));

	FSlateImageBrush LeftDefaultBrush = FSlateImageBrush(LeftShieldDefaultImage, FVector2D(0, 0));
	FSlateImageBrush RightDefaultBrush = FSlateImageBrush(RightShieldDefaultImage, FVector2D(0, 0));

	if(Shields == 0.0f)
	{
		// Adds a tint to shield bar images
		LeftShieldBar->WidgetStyle.SetBackgroundImage(LeftWarningBrush);
		RightShieldBar->WidgetStyle.SetBackgroundImage(RightWarningBrush);

		// Starts animation and loops infinately
		WarningText->SetVisibility(ESlateVisibility::Visible);
		WarningText->SetRenderOpacity(1.0f);
		PlayAnimation(WarningTextFadeAnimation, 0.0f, 0, EUMGSequencePlayMode::Forward, 1.0f);

	}

	else
	{
		// Resets shield bar images
		LeftShieldBar->WidgetStyle.SetBackgroundImage(LeftDefaultBrush);
		RightShieldBar->WidgetStyle.SetBackgroundImage(RightDefaultBrush);

		// Stops Animation
		StopAnimation(WarningTextFadeAnimation);
		WarningText->SetVisibility(ESlateVisibility::Hidden);
		WarningText->SetRenderOpacity(0.0f);
	}
}


void UC_PlayerHUDWidget::UpdateWeaponImages()
{
	APlayerController* const PC = Cast<APlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if(PC)
	{
		AC_PlayerCharacter* Player = Cast<AC_PlayerCharacter>(PC->GetPawn());
		if (Player)
		{
			//AC_BaseGun* Weapon = Cast<AC_BaseGun>(Player->EquippedWeaponArray[0]);
			if(Player->EquippedWeaponArray[0] && Player->EquippedWeaponArray[1])
			{
				PrimaryWeaponImage->SetBrushFromTexture(Player->EquippedWeaponArray[0]->WeaponIcon);
				SecondaryWeaponImage->SetBrushFromTexture(Player->EquippedWeaponArray[1]->WeaponIcon);
			}
		}
	}
}

void UC_PlayerHUDWidget::UpdateAmmoText()
{
	AC_PlayerCharacter* Player = Cast<AC_PlayerCharacter>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetPawn());
	if (Player)
	{
		AC_BaseGun* PrimaryWeapon = Cast<AC_BaseGun>(Player->EquippedWeaponArray[0]);
		AC_BaseGun* SecondaryWeapon = Cast<AC_BaseGun>(Player->EquippedWeaponArray[1]);

		if(PrimaryWeapon)
		{
			PrimaryMaxAmmoText->SetText(FText::FromString(FString::FromInt(PrimaryWeapon->WeaponStats.MaxReservesAmmo)));
		}

		if(SecondaryWeapon)
		{
			SecondaryMaxAmmoText->SetText(FText::FromString(FString::FromInt(SecondaryWeapon->WeaponStats.MaxReservesAmmo)));
		}
	}
}