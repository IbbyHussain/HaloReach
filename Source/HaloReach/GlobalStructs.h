#pragma once

#include "Engine.h"
#include "GlobalStructs.generated.h"

USTRUCT(BlueprintType)
struct FDirectionsStruct
{
	GENERATED_BODY()
public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Widget")
	FName DirectionName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Widget")
	int WorldRotation;

	FDirectionsStruct()
	{
		DirectionName = "";
		WorldRotation = 0;
	}

};