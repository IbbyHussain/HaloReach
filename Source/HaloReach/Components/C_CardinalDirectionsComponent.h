// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HaloReach/GlobalStructs.h"
#include "C_CardinalDirectionsComponent.generated.h"

USTRUCT(BlueprintType)
struct FTranslationsOutput
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Struct")
	FVector2D Translation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Struct")
	bool bIsClockwise;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Struct")
	bool bInRadarSight;

};

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Struct")
	UCameraComponent* PlayerCameraComp;

	float MaxWidgetTranslation = 332.5f;

# pragma region Helper Functions

	// UE4 uses rotation in range of 0 to 180 then -180 to 0.
	// This function will convert that range to 0 to 360.
	UFUNCTION(BlueprintCallable)
	float RotationToCirlceDegrees(float Rotation);

	UFUNCTION(BlueprintCallable)
	float GetDeltaClockwise(float A, float B, bool bIsClockwise);

	UFUNCTION(BlueprintCallable)
	FTranslationsOutput RotationToTranslation(FRotator A, FRotator B);

# pragma endregion
};
