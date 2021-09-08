
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "C_DeathUpdateWidget.generated.h"

class UTextBlock;
class UVerticalBox;
class UC_GlobalAlertWidget;

UCLASS()
class HALOREACH_API UC_DeathUpdateWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UC_DeathUpdateWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UVerticalBox* AlertBox;

	void UpdateAlertBox(FString A, FString B);

# pragma region Global Alert Widget

	void CreateGlobalAlertWidget();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UC_GlobalAlertWidget> GlobalAlertWidgetClass;

	UPROPERTY()
	UC_GlobalAlertWidget* GlobalAlertWidget;

	UPROPERTY()
	TArray<UWidget*> CurrentChildrenArray;

# pragma endregion
	
};
