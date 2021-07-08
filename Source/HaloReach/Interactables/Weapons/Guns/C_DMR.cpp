// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/Interactables/Weapons/Guns/C_DMR.h"

AC_DMR::AC_DMR()
{
	bAutoWeapon = false;
}

void AC_DMR::BeginPlay()
{
	Super::BeginPlay();

	Type = EWeaponType::RIFLE;
}
