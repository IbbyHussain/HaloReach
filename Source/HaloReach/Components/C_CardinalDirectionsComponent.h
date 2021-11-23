// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HaloReach/GlobalStructs.h"
#include "C_CardinalDirectionsComponent.generated.h"


UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class HALOREACH_API UC_CardinalDirectionsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UC_CardinalDirectionsComponent();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void InitDirections();

	class UC_CardinalDirectionsWidget* CardinalWidgetptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Widget")
	TArray<FDirectionsStruct> DirectionsArray;
};
