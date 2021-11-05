// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "C_NameInputWidget.generated.h"

class UTextBlock;
class UEditableTextBox;

UCLASS()
class HALOREACH_API UC_NameInputWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UC_NameInputWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* PlayerNameHintText;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	//UEditableTextBox* PlayerNameInputTextBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UEditableTextBox* PlayerNameInputTextBox;

	FString TestName();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* PlayerNameText;

	UFUNCTION()
	void OnPlayerNameComitted(const FText& InText, ETextCommit::Type InCommitType);

	//
	UFUNCTION(Client, Reliable)
	void Client_LoadPlayerName(const FString& SlotName);
	void Client_LoadPlayerName_Implementation(const FString& SlotName);

	UFUNCTION(Client, Reliable)
	void Client_SavePlayerName(const FString& SlotName, const FString& NewPlayerName);
	void Client_SavePlayerName_Implementation(const FString& SlotName, const FString& NewPlayerName);

};
