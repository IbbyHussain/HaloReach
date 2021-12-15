// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "C_DamageInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UC_DamageInterface : public UInterface
{
	GENERATED_BODY()
};

class HALOREACH_API IC_DamageInterface
{
	GENERATED_BODY()

public:

	UFUNCTION()
	virtual void DealDamage(AActor* DamagedActor, float BaseDamage, AController* EventInstigator, AActor* DamageCauser) = 0;

	UFUNCTION(BlueprintNativeEvent, Category = "Test")
	void DamageActor(AActor* DamagedActor, float BaseDamage, AController* EventInstigator, AActor* DamageCauser);
};
