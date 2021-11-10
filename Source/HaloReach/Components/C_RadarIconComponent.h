// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "C_RadarIconComponent.generated.h"


UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class HALOREACH_API UC_RadarIconComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UC_RadarIconComponent();

	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, Category = "Components")
	AActor* Owner;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Components")
	UTexture2D* RadarIconImage;

	// Will add a radar icon to the players radar widget
	UFUNCTION(BlueprintCallable)
	void AddRadarIcon();

	class AC_PlayerHUD* HUD;

	UFUNCTION(BlueprintImplementableEvent, Category = "BaseCharacter")
	void SetRadar();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class UC_RadarIcon* RadarIcon;
		
};
