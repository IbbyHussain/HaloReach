// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HaloReach/Interactables/Weapons/C_BaseWeapon.h"
#include "C_BaseGun.generated.h"

UCLASS()
class HALOREACH_API AC_BaseGun : public AC_BaseWeapon
{
	GENERATED_BODY()

protected:
	AC_BaseGun();

	UFUNCTION(BlueprintCallable)
	void Fire();

	virtual void BeginPlay() override;

	virtual void Attack() override;

	virtual void StopAttack() override;

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

	void PlayFireEffects();

	UPROPERTY(EditDefaultsOnly, Category = "Hit")
	TSubclassOf<UMatineeCameraShake> FireShake;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "hit")
	float BaseDamage;

	float LastFireTime;

	UPROPERTY(EditDefaultsOnly, Category = "Hit")
	float RateOfFire;

	float TimeBetweenShots;

// AUTOMATIC

	void StartFire();

	void StopFire();

	FTimerHandle AutomaticFireHandle;

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

	
};
