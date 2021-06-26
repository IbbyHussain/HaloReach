// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HaloReach/Interactables/Weapons/Guns/C_BaseGun.h"
#include "C_Magnum.generated.h"


UCLASS()
class HALOREACH_API AC_Magnum : public AC_BaseGun
{
	GENERATED_BODY()

private:

	AC_Magnum();

	virtual void BeginPlay() override;

	
};
