// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HaloReach/Interactables/Weapons/C_BaseWeapon.h"
#include "C_BaseGun.generated.h"

USTRUCT(BlueprintType)
struct FWeaponStats
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, Category = "Base Weapon | Weapon Stats")
	float BaseDamage;

	// RPM
	UPROPERTY(EditDefaultsOnly, Category = "Base Weapon | Weapon Stats")
	float RateOfFire;

	UPROPERTY(EditDefaultsOnly, Category = "Base Weapon | Weapon Stats")
	float ReloadSpeed;

	// Max ammo in a magazine
	UPROPERTY(EditDefaultsOnly, Category = "Base Weapon | Weapon Stats")
	int32 MaxMagazineAmmo;

	// Max ammo in reserves
	UPROPERTY(EditDefaultsOnly, Category = "Base Weapon | Weapon Stats")
	int32 MaxReservesAmmo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Base Weapon | Weapon Stats")
	int32 CurrentAmmo;

	float LastFireTime;

	float TimeBetweenShots;

	FWeaponStats()
	{
		BaseDamage = 10.0f;
		RateOfFire = 600.0f;

		ReloadSpeed = 1.0f;
		MaxMagazineAmmo = 32;
		MaxReservesAmmo = 224;

		CurrentAmmo = MaxMagazineAmmo;
	}
};

UCLASS()
class HALOREACH_API AC_BaseGun : public AC_BaseWeapon
{
	GENERATED_BODY()

protected:

	

	AC_BaseGun();

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "hit")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "hit")
	UParticleSystem* MuzzleEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "hit")
	UParticleSystem* DefaultImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "hit")
	UParticleSystem* FleshImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "hit")
	FName MuzzleSocketName;

	UPROPERTY(EditDefaultsOnly, Category = "Hit")
	TSubclassOf<UMatineeCameraShake> FireShake;

	// A basic fire function which uses a line trace and applies damage
	void Fire();

	// Plays particle effects for weapon
	void PlayFireEffects();

	void Reload();

// AUTOMATIC

	void StartAutoFire();
	void StopAutoFire();

	FTimerHandle AutomaticFireHandle;

// SEMI

	void StartSemiFire();

	FTimerHandle SemiFireHandle;

// Replication

	UFUNCTION(Server, Reliable)
	void Server_Fire();
	void Server_Fire_Implementation();

	UFUNCTION(NetMultiCast, Reliable)
	void Multi_Fire();
	void Multi_Fire_Implementation();

	UFUNCTION(Server, Reliable)
	void Server_StopFire();
	void Server_StopFire_Implementation();

	UFUNCTION(NetMultiCast, Reliable)
	void Multi_StopFire();
	void Multi_StopFire_Implementation();


private:

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "hello")
	FWeaponStats WeaponStats;
	
};
