// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/UI/C_NameInputWidget.h"
#include "C_NameInputWidget.h"
#include "Kismet/GameplayStatics.h"
#include "HaloReach/Player/PlayerExtra/C_ReachPlayerController.h"
#include "InputCoreTypes.h"
#include "GameFramework/PlayerState.h"
#include "HaloReach/Player/C_PlayerCharacter.h"
#include "HaloReach/UI/HUD/C_PlayerHUD.h"
#include "Components/EditableTextBox.h"
#include "Components/MultiLineEditableTextBox.h"

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

	PlayerNameInputTextBox->OnTextCommitted.AddDynamic(this, &UC_NameInputWidget::OnPlayerNameComitted);
}

void UC_NameInputWidget::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);

	//APlayerController* PC = (UGameplayStatics::GetPlayerController(GetWorld(), 0));
	//AC_PlayerHUD* HUD = Cast<AC_PlayerHUD>(PC->GetHUD());

	//if(PC->IsInputKeyDown(EKeys::Enter))
	//{
	//	AC_PlayerCharacter* PlayerCharacter = Cast<AC_PlayerCharacter>(PC->GetPawn());

	//	PlayerCharacter->bIsDead = false;
	//	PlayerCharacter->UpdateMovementSettings(EMovementState::NOTIDLE);
	//	PC->SetIgnoreMoveInput(false);

	//	PlayerCharacter->Server_SetPlayerName(PlayerNameInputTextBox->GetText().ToString());

	//	//AC_ReachPlayerController* RPC = Cast<AC_ReachPlayerController>(PC);
	//	//RPC->AssignedName = PlayerNameInputTextBox->GetText().ToString();
	//	//RPC->AssignedName = PlayerNameInputTextBox->GetText().ToString();
	//	//PC->PlayerState->SetPlayerName(PlayerNameInputTextBox->GetText().ToString());

	//	

	//	PC->SetInputMode(FInputModeGameOnly());
	//	PC->SetShowMouseCursor(false);
	//	HUD->DestroyNameInputWidget();
	//}

}


FString UC_NameInputWidget::TestName()
{
	// fails , passes string succeeds
	FString x = PlayerNameInputTextBox->GetText().ToString();
	UE_LOG(LogTemp, Error, TEXT("X IS: %s"), *x);
	return x; //PlayerNameInputTextBox->GetText().ToString()
}

void UC_NameInputWidget::OnPlayerNameComitted(const FText& InText, ETextCommit::Type InCommitType)
{
	switch (InCommitType)
	{
	case ETextCommit::OnEnter:
		
		FString PlayerInput = PlayerNameInputTextBox->GetText().ToString();

		// Cannot Submit empty name or name longer than 5 characters
		if(PlayerInput != "")
		{

			if(PlayerInput.Len() <= 5)
			{
				APlayerController* PC = (UGameplayStatics::GetPlayerController(GetWorld(), 0));
				AC_PlayerHUD* HUD = Cast<AC_PlayerHUD>(PC->GetHUD());

				AC_PlayerCharacter* PlayerCharacter = Cast<AC_PlayerCharacter>(PC->GetPawn());

				PlayerCharacter->bIsDead = false;
				PlayerCharacter->UpdateMovementSettings(EMovementState::NOTIDLE);
				PC->SetIgnoreMoveInput(false);

				PlayerCharacter->Server_SetPlayerName(PlayerInput);

				//AC_ReachPlayerController* RPC = Cast<AC_ReachPlayerController>(PC);
				//RPC->AssignedName = PlayerNameInputTextBox->GetText().ToString();
				//RPC->AssignedName = PlayerNameInputTextBox->GetText().ToString();
				//PC->PlayerState->SetPlayerName(PlayerNameInputTextBox->GetText().ToString());



				PC->SetInputMode(FInputModeGameOnly());
				PC->SetShowMouseCursor(false);
				HUD->DestroyNameInputWidget();
			}

			else
			{
				PlayerNameInputTextBox->SetHintText(FText::FromString("Player name is too long, maximum of 5 characters allowed"));
				PlayerNameInputTextBox->SetText(FText::FromString(""));
			}
			
		}

		else
		{
			PlayerNameInputTextBox->SetHintText(FText::FromString("Must enter a player name"));
		}
		
	}
}
