// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HaloReach/Interactables/C_WeaponPickupBase.h"
#include "C_DMRPickup.generated.h"

UCLASS()
class HALOREACH_API AC_DMRPickup : public AC_WeaponPickupBase
{
	GENERATED_BODY()

private:

	AC_DMRPickup();

	virtual void OnInteract() override;
};
