// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Runtime/UMG/Public/UMG.h"
#include "C_Radar.generated.h"

class UC_RadarIcon;

UCLASS()
class HALOREACH_API UC_Radar : public UUserWidget
{
	GENERATED_BODY()

		
public:

	UC_Radar(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;

	//Set the material parameter collection from the editor
	UPROPERTY(EditAnywhere)
	UMaterialParameterCollection* Collection;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UOverlay* MapOverlay;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UImage* Map;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Size")
	float Dimensions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Size")
	float Zoom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Size")
	class UTexture* MapTexture;

	//Player Icon widget
	UPROPERTY(EditAnywhere, Category = "Widgets")
	TSubclassOf<UUserWidget> PlayerIconClass;

	UPROPERTY(BlueprintReadWrite, Category = "Size")
	class UOverlaySlot* SlotRef;

	//Radar Icon widget
	UPROPERTY(EditAnywhere, Category = "Widgets")
	TSubclassOf<UC_RadarIcon> RadarIconClass;

	//Add point of intrest

	UFUNCTION(BlueprintCallable)
	UC_RadarIcon* AddPOI(AActor* Actor);

	UFUNCTION(BlueprintCallable)
	void RemovePOI();

	float CalculateZoom();

	UPROPERTY(BlueprintReadWrite, Category = "BP")
		UC_RadarIcon* RadarIcon;

private:

	UMaterialInstanceDynamic* DynamicMaterial;

	UMaterialParameterCollectionInstance* Instance;

	//UUserWidget* PlayerIcon;

	UOverlaySlot* MapOverlaySlot;



	
};
