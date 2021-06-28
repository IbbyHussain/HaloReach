// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/Interactables/Weapons/Guns/C_Magnum.h"

AC_Magnum::AC_Magnum()
{

}

void AC_Magnum::BeginPlay()
{
	Super::BeginPlay();

	Type = EWeaponType::PISTOL;


}

void AC_Magnum::Attack()
{
	StartSemiFire();
}
