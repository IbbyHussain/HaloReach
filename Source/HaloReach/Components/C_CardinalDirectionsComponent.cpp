
#include "HaloReach/Components/C_CardinalDirectionsComponent.h"
#include "Kismet/GameplayStatics.h"
#include "HaloReach/UI/HUD/C_PlayerHUD.h"
#include "HaloReach/UI/HUD/C_PlayerHUDWidget.h"
#include "HaloReach/Player/C_PlayerCharacter.h"

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

void UC_CardinalDirectionsComponent::InitDirections()
{
	AC_PlayerHUD* HUD = Cast<AC_PlayerHUD>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD());
	if(HUD)
	{
		CardinalWidgetptr = HUD->HUDWidget->CardinalWidget;
		if(CardinalWidgetptr)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("texthere: valid ptr")));
		}
	}
	
}