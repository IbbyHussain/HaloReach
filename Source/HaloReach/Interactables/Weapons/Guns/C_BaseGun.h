// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HaloReach/Interactables/Weapons/C_BaseWeapon.h"
#include "C_BaseGun.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFireWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStopFireWeapon);


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

	// Time of reload animation 
	UPROPERTY(EditDefaultsOnly, Category = "Base Weapon | Weapon Stats")
	float ReloadLength;

	UPROPERTY(EditDefaultsOnly, Category = "Base Weapon | Weapon Stats")
	float SwitchLength;

	// Max ammo in a magazine
	UPROPERTY(EditDefaultsOnly, Category = "Base Weapon | Weapon Stats")
	int32 MaxMagazineAmmo;

	// Max ammo in reserves
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Base Weapon | Weapon Stats")
	int32 MaxReservesAmmo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Base Weapon | Weapon Stats")
	int32 CurrentAmmo;

	UPROPERTY(EditDefaultsOnly, Category = "Base Weapon | Weapon Stats")
	bool bCanZoom;

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

// AMMO COUNTER

	UPROPERTY(EditDefaultsOnly, Category = "Base Weapon")
	TArray<UTexture2D*> AmmoCounterImageArray;

	void SetCurrentAmmoImage(UTexture2D* NewTexture);

	virtual void UpdateAmmoCounter();

// AUTOMATIC

	void StartAutoFire();
	void StopAutoFire();

	FTimerHandle AutomaticFireHandle;

// SEMI

	void StartSemiFire();

	FTimerHandle SemiFireHandle;

// RELOAD

	FTimerHandle ReloadHandle;

	bool bCanFire;

	void ResetCanFire();

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

	UTexture2D* CurrentAmmoImage;

	void Reload();

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnFireWeapon OnFireWeapon;

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnStopFireWeapon OnStopFireWeapon;

	
};
