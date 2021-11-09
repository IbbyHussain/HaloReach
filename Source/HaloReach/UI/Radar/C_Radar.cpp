// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/UI/Radar/C_Radar.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "Components/Overlay.h"
#include "Components/Image.h"
//#include "MyProject/UI/C_PlayerIcon.h"
//#include "MyProject/UI/C_PointOfIntrest.h"
//#include "MyProject/Components/C_POIComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "HaloReach/UI/Radar/C_RadarIcon.h"
#include "HaloReach/Libraries/C_SpawnLibrary.h"

UC_Radar::UC_Radar(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UC_Radar::NativeConstruct()
{
	Super::NativeConstruct();

	Instance = GetWorld()->GetParameterCollectionInstance(Collection);
	Instance->SetScalarParameterValue(FName("Dimensions"), Dimensions);
	Instance->SetScalarParameterValue(FName("Zoom"), Zoom);

	DynamicMaterial = Map->GetDynamicMaterial();
	DynamicMaterial->SetTextureParameterValue(FName("CurrentMap"), MapTexture);

	// PLAYER ICONS ARE NO LONGER USED - DEPRECATED

	//Creates the player icon class
	//PlayerIcon = UC_SpawnLibrary::SpawnWidgetNoViewport(GetWorld(), PlayerIconClass, PlayerIcon);
		
	//Adds the widget above the map
	//SlotRef = MapOverlay->AddChildToOverlay(PlayerIcon);

	//Sets it alignment to the center
	//SlotRef->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
	//SlotRef->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
		
	
}

void UC_Radar::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	if (UGameplayStatics::GetPlayerCharacter(this, 0))
	{
		//Sets the players X and Y co-ordinates to the Player icon's X and Y co-ordinates.
		float X = UGameplayStatics::GetPlayerCharacter(this, 0)->GetActorLocation().X;
		float Y = UGameplayStatics::GetPlayerCharacter(this, 0)->GetActorLocation().Y;

		// Makes the map move
		//Instance->SetScalarParameterValue(FName("X"), X);
		//Instance->SetScalarParameterValue(FName("Y"), Y);

		Instance->SetScalarParameterValue(FName("Zoom"), Zoom);
	}
}

UC_RadarIcon* UC_Radar::AddPOI(AActor* Actor)
{
	RadarIcon = UC_SpawnLibrary::SpawnWidgetNoViewport(GetWorld(), RadarIconClass, RadarIcon);

	RadarIcon->Owner = Actor;
	MapOverlaySlot = MapOverlay->AddChildToOverlay(RadarIcon);
	MapOverlaySlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
	MapOverlaySlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
	return RadarIcon;
}

void UC_Radar::RemovePOI()
{
	if(RadarIcon)
	{
		RadarIcon->RemoveFromParent();
	}
}

float UC_Radar::CalculateZoom()
{
	return Zoom * (Dimensions / 300.0f); // Default was 300
}
