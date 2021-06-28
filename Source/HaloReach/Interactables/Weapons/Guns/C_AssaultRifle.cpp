// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/Interactables/Weapons/Guns/C_AssaultRifle.h"

AC_AssaultRifle::AC_AssaultRifle()
{

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
