// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/Interactables/Weapons/C_Weapon3P.h"

// Sets default values
AC_Weapon3P::AC_Weapon3P()
{
 	WeaponMesh3P = CreateDefaultSubobject < USkeletalMeshComponent>(TEXT("Mesh Comp"));
    RootComponent = WeaponMesh3P;
    WeaponMesh3P->bOwnerNoSee = true;
    WeaponMesh3P->bCastDynamicShadow = true;
    WeaponMesh3P->CastShadow = true;
}



