// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/UI/HUD/C_CrosshairWidget.h"
#include "Components/Border.h"
#include "Kismet/KismetMathLibrary.h"

UC_CrosshairWidget::UC_CrosshairWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

float UC_CrosshairWidget::AddCrosshairSpread(float Amount, float DefaultSpread, float MaxSpread)
{
	float TempSpread = crosshair_spread + Amount;
	crosshair_spread = FMath::Clamp(TempSpread, DefaultSpread, MaxSpread);
	return crosshair_spread;
}
