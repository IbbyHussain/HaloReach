// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/Interactables/Weapons/C_BaseWeapon.h"
#include "HaloReach/Player/C_PlayerCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "HaloReach/UI/HUD/C_PlayerHUD.h"

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

	Melee3PMontage->GetPlayLength();

	MeleeTime = Melee1PMontage->GetPlayLength() / Melee1PMontage->RateScale;
}

UAnimMontage* AC_BaseWeapon::GetWeaponEquipMontage()
{
	return WeaponEquipAnimation;
}

UAnimMontage* AC_BaseWeapon::GetWeaponReloadMontage()
{
	return ReloadAnimation;
}

UAnimMontage* AC_BaseWeapon::GetWeaponFireMontage()
{
	return FireAnimation;
}

UAnimMontage* AC_BaseWeapon::GetWeapon1PMeleeMontage()
{
	return Melee1PMontage;
}

UAnimMontage* AC_BaseWeapon::GetWeapon3PEquipMontage()
{
	return Equip3PMontage;
}

UAnimMontage* AC_BaseWeapon::GetWeapon3PReloadMontage()
{
	return Reload3PMontage;
}

UAnimMontage* AC_BaseWeapon::GetWeapon3PFireMontage()
{
	return Fire3PMontage;
}

UAnimMontage* AC_BaseWeapon::GetWeapon3PMeleeMontage()
{
	return Melee3PMontage;
}

void AC_BaseWeapon::Attack()
{
	//UE_LOG(LogTemp, Log, TEXT("Attacked!"));
	AC_PlayerHUD* HUD = Cast<AC_PlayerHUD>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD());
	if(HUD)
	{
		HUD->AddCrosshairSpreadHUD(10.0f);
	}

}

void AC_BaseWeapon::StopAttack()
{

}