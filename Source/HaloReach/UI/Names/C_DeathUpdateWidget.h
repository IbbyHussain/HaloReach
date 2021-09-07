
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "C_DeathUpdateWidget.generated.h"

class UTextBlock;
class UVerticalBox;

UCLASS()
class HALOREACH_API UC_DeathUpdateWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UC_DeathUpdateWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* DeathUpdateText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UVerticalBox* AlertBox;

	void UpdateDeathText(FString NewText);

	void UpdateAlertBox(UWidget* AlertWidget);
	
};
