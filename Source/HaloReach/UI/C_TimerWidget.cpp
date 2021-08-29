// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/UI/C_TimerWidget.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "HaloReach/GameModes/C_BaseReachGameMode.h"

UC_TimerWidget::UC_TimerWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	Minutes = MaxMinutes;
	Seconds = MaxSeconds;
}

void UC_TimerWidget::NativeConstruct()
{
	HUDTextColour = FSlateColor(FColor(255, 0, 0));

	StartTimer();
}

void UC_TimerWidget::StartTimer()
{
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UC_TimerWidget::UpdateTime, 1.0f, true);
}

void UC_TimerWidget::UpdateTime()
{
	if(Seconds == 0)
	{
		if(Minutes == 0)
		{
			GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
			// finished timer is 0
		}

		else
		{
			Seconds = 59;
			Minutes -= 1;

			
		}
	}

	else
	{
		Seconds -= 1;
		
	}

	UpdateSeconds();
	UpdateMinutes();
}

void UC_TimerWidget::UpdateSeconds()
{
	SecondsTimerText->SetText(FText::FromString(FString::FromInt(Seconds)));
}

void UC_TimerWidget::UpdateMinutes()
{
	MinutesTimerText->SetText(FText::FromString(FString::FromInt(Minutes)));
}
