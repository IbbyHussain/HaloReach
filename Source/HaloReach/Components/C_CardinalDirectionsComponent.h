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

# pragma region Helper Functions

	// UE4 uses rotation in range of 0 to 180 then -180 to 0.
	// This function will convert that range to 0 to 360.
	UFUNCTION(BlueprintCallable)
	float RotationToCirlceDegrees(float Rotation);

# pragma endregion
};
