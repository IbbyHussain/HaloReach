// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "C_DamageLibrary.generated.h"

UCLASS()
class HALOREACH_API UC_DamageLibrary : public UObject
{
	GENERATED_BODY()

public:

	float DealDamage(AActor* DamagedActor, float BaseDamage, AController* EventInstigator, AActor* DamageCauser);

	void DealDamageAOE();
	
};
