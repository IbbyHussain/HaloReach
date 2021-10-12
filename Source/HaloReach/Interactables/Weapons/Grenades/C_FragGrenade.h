// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HaloReach/Interactables/Weapons/C_BaseGrenade.h"
#include "C_FragGrenade.generated.h"

UCLASS()
class HALOREACH_API AC_FragGrenade : public AC_BaseGrenade
{
	GENERATED_BODY()

	AC_FragGrenade();
	
	virtual void StartExplosion() override;
};
