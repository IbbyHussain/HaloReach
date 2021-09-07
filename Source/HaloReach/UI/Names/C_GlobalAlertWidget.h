// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "C_GlobalAlertWidget.generated.h"

class UTextBlock;

UCLASS()
class HALOREACH_API UC_GlobalAlertWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UC_GlobalAlertWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* AlertText;

	void SetAlertText(const FString UpdatedAlertText);

# pragma region Widget Animation

	void StoreWidgetAnimation();

	UWidgetAnimation* GetAnimationByName(FName AnimationName) const;

	TMap<FName, UWidgetAnimation*> AnimationsMap;

	UWidgetAnimation* FadeInAnimation;

	UWidgetAnimation* FadeOutAnimation;

	UFUNCTION()
	void PlayFadeInAnimation();

	UFUNCTION()
	void PlayFadeOutAnimation();

# pragma endregion

	FTimerHandle DestroyHandle;

	void DestroyWidget();

	FWidgetAnimationDynamicEvent EndDelegate;

	UFUNCTION()
	void AnimationFinished();

};
