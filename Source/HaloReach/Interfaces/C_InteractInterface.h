// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "C_InteractInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UC_InteractInterface : public UInterface
{
	GENERATED_BODY()
};


class HALOREACH_API IC_InteractInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Interact();
};
