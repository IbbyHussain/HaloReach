// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/Interactables/C_DMRPickup.h"

AC_DMRPickup::AC_DMRPickup()
{
	/*WeaponSocket1P = ("DMR_1P_Socket");
	WeaponSocket3P = ("DMR_3P_Socket");*/
}

void AC_DMRPickup::OnInteract()
{
	SpawnWeapon(WeaponClass);
}