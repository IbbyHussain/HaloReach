
#include "HaloReach/Interactables/C_WeaponPickupBase.h"
#include "HaloReach/Player/C_PlayerCharacter.h"
#include "HaloReach/Interactables/Weapons/C_BaseWeapon.h"


AC_WeaponPickupBase::AC_WeaponPickupBase()
{

}

void AC_WeaponPickupBase::SpawnWeapon(TSubclassOf<AC_BaseWeapon> WeaponClassToSpawn)
{
	if (Player)
	{
		if(Player->HasAuthority())
		{
			Player->UpdateWeapons();
			Player->SetupWeaponSwitching(WeaponClassToSpawn, WeaponSocket1P, WeaponSocket3P, false);
		}
		
		//Player->Update3PWeapons(WeaponSocket3P);
	}
}

void AC_WeaponPickupBase::OnOverlap()
{

}

void AC_WeaponPickupBase::OnOverlapEnd()
{

}

void AC_WeaponPickupBase::BeginPlay()
{
	Super::BeginPlay();

	Weapon = WeaponClass->GetDefaultObject<AC_BaseWeapon>();

	if(Weapon)
	{
		WeaponSocket1P = Weapon->Socket1P;
		WeaponSocket3P = Weapon->Socket3P;
	}
}