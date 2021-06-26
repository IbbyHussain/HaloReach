// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HaloReach/Interactables/C_WeaponPickupBase.h"
#include "C_ARPickup.generated.h"

UCLASS()
class HALOREACH_API AC_ARPickup : public AC_WeaponPickupBase
{
	GENERATED_BODY()

private:

	AC_ARPickup();

	virtual void OnInteract() override;
};
