// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "C_RadarIcon.generated.h"

class UC_Radar;
class UC_RadarIconComponent;

UCLASS()
class HALOREACH_API UC_RadarIcon : public UUserWidget
{
	GENERATED_BODY()

public:

	UC_RadarIcon(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;

	UPROPERTY(EditdefaultsOnly, BlueprintReadWrite, meta = (BindWidget))
	class UOverlay* DefaultOverlay;

	UPROPERTY(EditdefaultsOnly, BlueprintReadWrite, meta = (BindWidget))
	class UThrobber* DefaultImage;

	UPROPERTY(EditdefaultsOnly, BlueprintReadWrite, meta = (BindWidget))
	class USizeBox* Container;

	UPROPERTY(EditdefaultsOnly, BlueprintReadWrite, meta = (BindWidget))
	class UImage* IconImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "x")
	AActor* Owner;

	//Calculates the location of the POI, then sets it's location
	UFUNCTION(BlueprintCallable)
	FVector2D GetPOILocation(UC_Radar* MiniMap);

	//Updates the POI Image, so that it can be set per instance
	UFUNCTION(BlueprintCallable)
	void UpdateIconImage(UC_RadarIconComponent* POIComponent);

# pragma region Widget Animation

	void StoreWidgetAnimation();

	UWidgetAnimation* GetAnimationByName(FName AnimationName) const;

	TMap<FName, UWidgetAnimation*> AnimationsMap;

	UWidgetAnimation* FadeInAnimation;

	UWidgetAnimation* FadeOutAnimation;

	UFUNCTION(BlueprintCallable)
	void PlayFadeInAnimation();

	UFUNCTION(BlueprintCallable)
	void PlayFadeOutAnimation();

	void StopFadeOutAnimation();

# pragma endregion

private:

	// Finds the angle between the two locations (player and POI)
	//UFUNCTION(BlueprintCallable)
	float FindTheeta(FVector2D PointA, FVector2D PointB);

	//UFUNCTION(BlueprintCallable)
	FVector2D FindCoordinate(float Radius, float Degress);

	float DegCosY;
	float DegSinX;

	//Calculates the distance between these two locations
	TTuple<float, float>ActorLocation();

	float Returnx();

	float Returny();
};
