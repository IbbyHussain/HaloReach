// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/UI/HUD/C_CrosshairWidget.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Kismet/KismetMathLibrary.h"
#include "HaloReach/Player/C_PlayerCharacter.h"

UC_CrosshairWidget::UC_CrosshairWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	Team = 0;
}

void UC_CrosshairWidget::SwitchCrosshairOuter(AC_PlayerCharacter* Target)
{
	// Temporary for testing purposes, will need to update when team affiliation is implemented

	switch (Team)
	{
	case 0:
		// Default
		CrosshairOuter->SetBrushFromTexture(CrosshairOuterArray[0]);
		break;

	case 1:
		// Enemy
		CrosshairOuter->SetBrushFromTexture(CrosshairOuterArray[1]);
		break;

	case 2:
		// Friendly
		CrosshairOuter->SetBrushFromTexture(CrosshairOuterArray[2]);
		break;
	}
}

float UC_CrosshairWidget::AddCrosshairSpread(float Amount, float DefaultSpread, float MaxSpread)
{
	float TempSpread = crosshair_spread + Amount;
	crosshair_spread = FMath::Clamp(TempSpread, DefaultSpread, MaxSpread);
	return crosshair_spread;
}
