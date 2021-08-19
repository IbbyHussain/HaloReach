// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/UI/HUD/C_CrosshairWidget.h"
#include "Components/Border.h"

UC_CrosshairWidget::UC_CrosshairWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

float UC_CrosshairWidget::AddCrosshairSpread(float Amount)
{
	float x = crosshair_spread += Amount;
	return x;
}
