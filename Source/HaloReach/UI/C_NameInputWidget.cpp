// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/UI/C_NameInputWidget.h"
#include "C_NameInputWidget.h"
#include "Kismet/GameplayStatics.h"
#include "InputCoreTypes.h"
#include "GameFramework/PlayerState.h"
#include "HaloReach/Player/C_PlayerCharacter.h"
#include "HaloReach/UI/HUD/C_PlayerHUD.h"
#include "Components/EditableTextBox.h"

UC_NameInputWidget::UC_NameInputWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UC_NameInputWidget::NativeConstruct()
{
	Super::NativeConstruct();

	APlayerController* PC = (UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if(PC)
	{
		PC->SetInputMode(FInputModeGameAndUI());
		PC->SetShowMouseCursor(true);

		AC_PlayerCharacter* PlayerCharacter = Cast<AC_PlayerCharacter>(PC->GetPawn());
		PlayerCharacter->bIsDead = true;
		PlayerCharacter->UpdateMovementSettings(EMovementState::IDLE);
		PC->SetIgnoreMoveInput(true);
	}
}

void UC_NameInputWidget::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);

	APlayerController* PC = (UGameplayStatics::GetPlayerController(GetWorld(), 0));
	AC_PlayerHUD* HUD = Cast<AC_PlayerHUD>(PC->GetHUD());

	if(PC->IsInputKeyDown(EKeys::Enter))
	{
		AC_PlayerCharacter* PlayerCharacter = Cast<AC_PlayerCharacter>(PC->GetPawn());

		PlayerCharacter->bIsDead = false;
		PlayerCharacter->UpdateMovementSettings(EMovementState::NOTIDLE);
		PC->SetIgnoreMoveInput(false);

		PC->PlayerState->SetPlayerName(PlayerNameInputTextBox->GetText().ToString());

		PC->SetInputMode(FInputModeGameOnly());
		PC->SetShowMouseCursor(false);
		HUD->DestroyNameInputWidget();
	}

}



