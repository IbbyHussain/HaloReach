// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/Interactables/Weapons/Guns/C_AssaultRifle.h"

AC_AssaultRifle::AC_AssaultRifle()
{
	bAutoWeapon = true;
}

void AC_AssaultRifle::BeginPlay()
{
	Super::BeginPlay();

	Type = EWeaponType::RIFLE;

}

void AC_AssaultRifle::Attack()
{
	StartAutoFire();
}

void AC_AssaultRifle::StopAttack()
{
	StopAutoFire();
}

void AC_AssaultRifle::UpdateAmmoCounter()
{
	switch (WeaponStats.CurrentAmmo)
	{
	case 32:
		SetCurrentAmmoImage(AmmoCounterImageArray[32]);
		break;
	case 31:
		SetCurrentAmmoImage(AmmoCounterImageArray[31]);
		break;
	case 30:
		SetCurrentAmmoImage(AmmoCounterImageArray[30]);
		break;
	case 29:
		SetCurrentAmmoImage(AmmoCounterImageArray[29]);
		break;
	case 28:
		SetCurrentAmmoImage(AmmoCounterImageArray[28]);
		break;
	case 27:
		SetCurrentAmmoImage(AmmoCounterImageArray[27]);
		break;
	case 26:
		SetCurrentAmmoImage(AmmoCounterImageArray[26]);
		break;
	case 25:
		SetCurrentAmmoImage(AmmoCounterImageArray[25]);
		break;
	case 24:
		SetCurrentAmmoImage(AmmoCounterImageArray[24]);
		break;
	case 23:
		SetCurrentAmmoImage(AmmoCounterImageArray[23]);
		break;
	case 22:
		SetCurrentAmmoImage(AmmoCounterImageArray[22]);
		break;
	case 21:
		SetCurrentAmmoImage(AmmoCounterImageArray[21]);
		break;
	case 20:
		SetCurrentAmmoImage(AmmoCounterImageArray[20]);
		break;
	case 19:
		SetCurrentAmmoImage(AmmoCounterImageArray[19]);
		break;
	case 18:
		SetCurrentAmmoImage(AmmoCounterImageArray[18]);
		break;
	case 17:
		SetCurrentAmmoImage(AmmoCounterImageArray[17]);
		break;
	case 16:
		SetCurrentAmmoImage(AmmoCounterImageArray[16]);
		break;
	case 15:
		SetCurrentAmmoImage(AmmoCounterImageArray[15]);
		break;
	case 14:
		SetCurrentAmmoImage(AmmoCounterImageArray[14]);
		break;
	case 13:
		SetCurrentAmmoImage(AmmoCounterImageArray[13]);
		break;
	case 12:
		SetCurrentAmmoImage(AmmoCounterImageArray[12]);
		break;
	case 11:
		SetCurrentAmmoImage(AmmoCounterImageArray[11]);
		break;
	case 10:
		SetCurrentAmmoImage(AmmoCounterImageArray[10]);
		break;
	case 9:
		SetCurrentAmmoImage(AmmoCounterImageArray[9]);
		break;
	case 8:
		SetCurrentAmmoImage(AmmoCounterImageArray[8]);
		break;
	case 7:
		SetCurrentAmmoImage(AmmoCounterImageArray[7]);
		break;
	case 6:
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
