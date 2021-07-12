// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/Interactables/Weapons/Guns/C_AssaultRifle.h"

AC_AssaultRifle::AC_AssaultRifle() : bX(true)
{
	bAutoWeapon = true;

	Type = EWeaponType::RIFLE;
}

void AC_AssaultRifle::BeginPlay()
{
	Super::BeginPlay();

	

	//Set the weapon mesh to use this as the ammo dispaly material
	if(DisplayMaterialIndex != -1)
	{
		SetMeshAmmoCounter(MeshAmmoCounterArray[32]);
		WeaponMesh->SetMaterial(2, DynMaterial);
	}
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
		SetMeshAmmoCounter(MeshAmmoCounterArray[32]);
		break;
	case 31:
		SetCurrentAmmoImage(AmmoCounterImageArray[31]);
		SetMeshAmmoCounter(MeshAmmoCounterArray[31]);
		break;
	case 30:
		SetCurrentAmmoImage(AmmoCounterImageArray[30]);
		SetMeshAmmoCounter(MeshAmmoCounterArray[30]);
		break;
	case 29:
		SetCurrentAmmoImage(AmmoCounterImageArray[29]);
		SetMeshAmmoCounter(MeshAmmoCounterArray[29]);
		break;
	case 28:
		SetCurrentAmmoImage(AmmoCounterImageArray[28]);
		SetMeshAmmoCounter(MeshAmmoCounterArray[28]);
		break;
	case 27:
		SetCurrentAmmoImage(AmmoCounterImageArray[27]);
		SetMeshAmmoCounter(MeshAmmoCounterArray[27]);
		break;
	case 26:
		SetCurrentAmmoImage(AmmoCounterImageArray[26]);
		SetMeshAmmoCounter(MeshAmmoCounterArray[26]);
		break;
	case 25:
		SetCurrentAmmoImage(AmmoCounterImageArray[25]);
		SetMeshAmmoCounter(MeshAmmoCounterArray[25]);
		break;
	case 24:
		SetCurrentAmmoImage(AmmoCounterImageArray[24]);
		SetMeshAmmoCounter(MeshAmmoCounterArray[24]);
		break;
	case 23:
		SetCurrentAmmoImage(AmmoCounterImageArray[23]);
		SetMeshAmmoCounter(MeshAmmoCounterArray[23]);
		break;
	case 22:
		SetCurrentAmmoImage(AmmoCounterImageArray[22]);
		SetMeshAmmoCounter(MeshAmmoCounterArray[22]);
		break;
	case 21:
		SetCurrentAmmoImage(AmmoCounterImageArray[21]);
		SetMeshAmmoCounter(MeshAmmoCounterArray[21]);
		break;
	case 20:
		SetCurrentAmmoImage(AmmoCounterImageArray[20]);
		SetMeshAmmoCounter(MeshAmmoCounterArray[20]);
		break;
	case 19:
		SetCurrentAmmoImage(AmmoCounterImageArray[19]);
		SetMeshAmmoCounter(MeshAmmoCounterArray[19]);
		break;
	case 18:
		SetCurrentAmmoImage(AmmoCounterImageArray[18]);
		SetMeshAmmoCounter(MeshAmmoCounterArray[18]);
		break;
	case 17:
		SetCurrentAmmoImage(AmmoCounterImageArray[17]);
		SetMeshAmmoCounter(MeshAmmoCounterArray[17]);
		break;
	case 16:
		SetCurrentAmmoImage(AmmoCounterImageArray[16]);
		SetMeshAmmoCounter(MeshAmmoCounterArray[16]);
		break;
	case 15:
		SetCurrentAmmoImage(AmmoCounterImageArray[15]);
		SetMeshAmmoCounter(MeshAmmoCounterArray[15]);
		break;
	case 14:
		SetCurrentAmmoImage(AmmoCounterImageArray[14]);
		SetMeshAmmoCounter(MeshAmmoCounterArray[14]);
		break;
	case 13:
		SetCurrentAmmoImage(AmmoCounterImageArray[13]);
		SetMeshAmmoCounter(MeshAmmoCounterArray[13]);
		break;
	case 12:
		SetCurrentAmmoImage(AmmoCounterImageArray[12]);
		SetMeshAmmoCounter(MeshAmmoCounterArray[12]);
		break;
	case 11:
		SetCurrentAmmoImage(AmmoCounterImageArray[11]);
		SetMeshAmmoCounter(MeshAmmoCounterArray[11]);
		break;
	case 10:
		SetCurrentAmmoImage(AmmoCounterImageArray[10]);
		SetMeshAmmoCounter(MeshAmmoCounterArray[10]);
		break;
	case 9:
		SetCurrentAmmoImage(AmmoCounterImageArray[9]);
		SetMeshAmmoCounter(MeshAmmoCounterArray[9]);
		break;
	case 8:
		SetCurrentAmmoImage(AmmoCounterImageArray[8]);
		SetMeshAmmoCounter(MeshAmmoCounterArray[8]);
		break;
	case 7:
		SetCurrentAmmoImage(AmmoCounterImageArray[7]);
		SetMeshAmmoCounter(MeshAmmoCounterArray[7]);
		break;
	case 6:
		SetCurrentAmmoImage(AmmoCounterImageArray[6]);
		SetMeshAmmoCounter(MeshAmmoCounterArray[6]);
		break;
	case 5:
		SetCurrentAmmoImage(AmmoCounterImageArray[5]);
		SetMeshAmmoCounter(MeshAmmoCounterArray[5]);
		break;
	case 4:
		SetCurrentAmmoImage(AmmoCounterImageArray[4]);
		SetMeshAmmoCounter(MeshAmmoCounterArray[4]);
		break;
	case 3:
		SetCurrentAmmoImage(AmmoCounterImageArray[3]);
		SetMeshAmmoCounter(MeshAmmoCounterArray[3]);
		break;
	case 2:
		SetCurrentAmmoImage(AmmoCounterImageArray[2]);
		SetMeshAmmoCounter(MeshAmmoCounterArray[2]);
		break;
	case 1:
		SetCurrentAmmoImage(AmmoCounterImageArray[1]);
		SetMeshAmmoCounter(MeshAmmoCounterArray[1]);
		break;
	case 0:
		SetCurrentAmmoImage(AmmoCounterImageArray[0]);
		SetMeshAmmoCounter(MeshAmmoCounterArray[0]);
		break;
	}
}
