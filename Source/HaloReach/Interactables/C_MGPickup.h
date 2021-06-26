// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HaloReach/Interactables/C_WeaponPickupBase.h"
#include "C_MGPickup.generated.h"

UCLASS()
class HALOREACH_API AC_MGPickup : public AC_WeaponPickupBase
{
	GENERATED_BODY()
	
private:
	AC_MGPickup();

	virtual void OnInteract() override;
};
