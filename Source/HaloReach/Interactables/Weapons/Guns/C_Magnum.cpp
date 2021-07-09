// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/Interactables/Weapons/Guns/C_Magnum.h"

AC_Magnum::AC_Magnum()
{
	bAutoWeapon = false;

	WeaponStats.CurrentAmmo = 8;
	WeaponStats.MaxMagazineAmmo = 8;
	WeaponStats.MaxReservesAmmo = 64;
}

void AC_Magnum::BeginPlay()
{
	Super::BeginPlay();

	Type = EWeaponType::PISTOL;


}

void AC_Magnum::Attack()
{
	StartSemiFire();

	UpdateAmmoCounter();
}

void AC_Magnum::UpdateAmmoCounter()
{
	switch (WeaponStats.CurrentAmmo)
	{
	case 8:
		UE_LOG(LogTemp, Log, TEXT("8 in chamber"));
		SetCurrentAmmoImage(AmmoCounterImageArray[8]);
		break;
	case 7:
		UE_LOG(LogTemp, Log, TEXT("7 in chamber"));
		SetCurrentAmmoImage(AmmoCounterImageArray[7]);
		break;
	case 6:
		UE_LOG(LogTemp, Log, TEXT("6 in chamber"));
		SetCurrentAmmoImage(AmmoCounterImageArray[6]);
		break;
	case 5:
		SetCurrentAmmoImage(AmmoCounterImageArray[5]);
		break;
	case 4:
		SetCurrentAmmoImage(AmmoCounterImageArray[4]);
		break;
	case 3:
		SetCurrentAmmoImage(AmmoCounterImageArray[3]);
		break;
	case 2:
		SetCurrentAmmoImage(AmmoCounterImageArray[2]);
		break;
	case 1:
		SetCurrentAmmoImage(AmmoCounterImageArray[1]);
		break;
	case 0:
		SetCurrentAmmoImage(AmmoCounterImageArray[0]);
		break;
	}
}
