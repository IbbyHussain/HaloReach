
#pragma once

#include "CoreMinimal.h"
#include "HaloReach/Interactables/C_BasePickup.h"
#include "C_WeaponPickupBase.generated.h"


UCLASS()
class HALOREACH_API AC_WeaponPickupBase : public AC_BasePickup
{
	GENERATED_BODY()

protected:

	AC_WeaponPickupBase();

// SPAWNING WEAPON
	void SpawnWeapon(TSubclassOf<AC_BaseWeapon> WeaponClassToSpawn);

	UPROPERTY(EditDefaultsOnly, Category = "Pickup")
	TSubclassOf<AC_BaseWeapon> WeaponClass;

	AC_BaseWeapon* Weapon;

	FName WeaponSocket1P, WeaponSocket3P;

	virtual void OnOverlap() override;

	virtual void OnOverlapEnd() override;

	virtual void BeginPlay() override;
	
};
