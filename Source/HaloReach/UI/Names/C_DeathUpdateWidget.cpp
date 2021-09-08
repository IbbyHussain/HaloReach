// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/UI/Names/C_DeathUpdateWidget.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "HaloReach/Libraries/C_SpawnLibrary.h"
#include "HaloReach/UI/Names/C_GlobalAlertWidget.h"

UC_DeathUpdateWidget::UC_DeathUpdateWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
}

void UC_DeathUpdateWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UC_DeathUpdateWidget::UpdateAlertBox(FString A, FString B)
{
	//AlertBox->AddChild(AlertWidget);

	if(AlertBox->HasAnyChildren())
	{
		CurrentChildrenArray = AlertBox->GetAllChildren();
		AlertBox->ClearChildren();
		CreateGlobalAlertWidget();
		GlobalAlertWidget->SetAlertText(FString::Printf(TEXT("%s Killed %s"), *A, *B));

		for (auto X : CurrentChildrenArray)
		{
			AlertBox->AddChild(X);
		}

	}

	else
	{
		CreateGlobalAlertWidget();
		GlobalAlertWidget->SetAlertText(FString::Printf(TEXT("%s Killed %s"), *A, *B));
	}
	

	//GEngine->AddOnScreenDebugMessage(-1, 4.5f, FColor::Magenta, FString::Printf(TEXT("Index is AFTR: %d"), AlertBox->GetChildIndex(AlertWidget)));
}

void UC_DeathUpdateWidget::CreateGlobalAlertWidget()
{
	GlobalAlertWidget = UC_SpawnLibrary::SpawnWidgetNoViewport(GetWorld(), GlobalAlertWidgetClass, GlobalAlertWidget);
	AlertBox->AddChild(GlobalAlertWidget);
}