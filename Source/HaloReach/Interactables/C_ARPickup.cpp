// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/Interactables/C_ARPickup.h"
#include "HaloReach/Player/C_PlayerCharacter.h"

AC_ARPickup::AC_ARPickup()
{
	/*WeaponSocket1P = ("R_GunSocket");
	WeaponSocket3P = ("ARSocket");*/
}

void AC_ARPickup::OnInteract()
{
	SpawnWeapon(WeaponClass);
}
