// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HaloReach/Interactables/Weapons/Guns/C_BaseGun.h"
#include "C_DMR.generated.h"

/**
 * 
 */
UCLASS()
class HALOREACH_API AC_DMR : public AC_BaseGun
{
	GENERATED_BODY()

private:

	AC_DMR();

	virtual void BeginPlay() override;



	
};
