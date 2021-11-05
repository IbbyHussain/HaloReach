// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/UI/C_NameInputWidget.h"
#include "C_NameInputWidget.h"
#include "Kismet/GameplayStatics.h"
#include "HaloReach/Player/PlayerExtra/C_ReachPlayerController.h"
#include "InputCoreTypes.h"
#include "GameFramework/PlayerState.h"
#include "HaloReach/Player/C_PlayerCharacter.h"
#include "HaloReach/UI/HUD/C_PlayerHUD.h"
#include "HaloReach/Player/PlayerExtra/C_ReachPlayerState.h"

#include "Components/EditableTextBox.h"
#include "Components/MultiLineEditableTextBox.h"
#include "Components/TextBlock.h"
#include "HaloReach/GameModes/C_ReachGameInstance.h"
#include "HaloReach/Interfaces/C_BaseSaveGame.h"
#include "GameFramework/PlayerState.h"


UC_NameInputWidget::UC_NameInputWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UC_NameInputWidget::NativeConstruct()
{
	Super::NativeConstruct();

	APlayerState* PS = UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayerState;
	if(PS)
	{
		//Client_LoadPlayerName(PS->GetPlayerName());
		PS->UniqueId->ToString();
		Client_LoadPlayerName(PS->UniqueId->ToString());
	}

	PlayerNameInputTextBox->OnTextCommitted.AddDynamic(this, &UC_NameInputWidget::OnPlayerNameComitted);
}

void UC_NameInputWidget::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);
}

FString UC_NameInputWidget::TestName()
{
	// fails , passes string succeeds
	FString x = PlayerNameInputTextBox->GetText().ToString();
	UE_LOG(LogTemp, Error, TEXT("X IS: %s"), *x);
	return x;
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
				PlayerNameText->SetText(FText::FromString("Player name is: " + PlayerInput));

				APlayerState* PS = UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayerState;
				if (PS)
				{
					Client_SavePlayerName(PS->UniqueId->ToString(), PlayerInput);
				}
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

void UC_NameInputWidget::Client_LoadPlayerName_Implementation(const FString& SlotName)
{
	// If the player has set a custom saved name
	if(UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		if (UC_BaseSaveGame* LoadedGame = Cast<UC_BaseSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0)))
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("LOAD GAME SUCCEEDED (Slot name is: %)"), *SlotName));
			PlayerNameText->SetText(FText::FromString("Player name is: " + LoadedGame->SavedPlayerName));
		}
	}

	// If the player has not set a custom saved name, will use steam name.
	else
	{
		APlayerState* PS = UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayerState;
		if(PS)
		{
			PlayerNameText->SetText(FText::FromString("Player name is: " + PS->GetPlayerName()));
		}
	}
}

void UC_NameInputWidget::Client_SavePlayerName_Implementation(const FString& SlotName, const FString& NewPlayerName)
{
	if(UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		if (UC_BaseSaveGame* LoadedGame = Cast<UC_BaseSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0)))
		{
			LoadedGame->SavedPlayerName = NewPlayerName;
			UGameplayStatics::SaveGameToSlot(LoadedGame, SlotName, 0);
		}
	}

	else
	{
		if (UC_BaseSaveGame* SaveGameInstance = Cast<UC_BaseSaveGame>(UGameplayStatics::CreateSaveGameObject(UC_BaseSaveGame::StaticClass())))
		{
			SaveGameInstance->SavedPlayerName = NewPlayerName;
			UGameplayStatics::SaveGameToSlot(SaveGameInstance, SlotName, 0);
		}
	}
}

