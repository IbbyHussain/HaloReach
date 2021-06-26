// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/Interactables/Weapons/C_BaseWeapon.h"
#include "HaloReach/Player/C_PlayerCharacter.h"

AC_BaseWeapon::AC_BaseWeapon()
{
	WeaponMesh = CreateDefaultSubobject < USkeletalMeshComponent>(TEXT("Mesh Comp"));
	RootComponent = WeaponMesh;
	WeaponMesh->SetOnlyOwnerSee(true);
	WeaponMesh->bCastDynamicShadow = false;
	WeaponMesh->CastShadow = false;

	Player = Cast<AC_PlayerCharacter>(GetOwner());

	bReplicates = true;
}


void AC_BaseWeapon::BeginPlay()
{
	Super::BeginPlay();
}

UAnimMontage* AC_BaseWeapon::GetWeaponEquipMontage()
{
	return WeaponEquipAnimation;
}

void AC_BaseWeapon::Attack()
{
	//UE_LOG(LogTemp, Log, TEXT("Attacked!"));
}

void AC_BaseWeapon::StopAttack()
{

}
