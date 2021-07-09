// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HaloReach/GlobalEnums.h"
#include "C_BaseWeapon.generated.h"

UCLASS()
class HALOREACH_API AC_BaseWeapon : public AActor
{
	GENERATED_BODY()
	
public:	

	AC_BaseWeapon();

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Base Weapon | General")
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Base Weapon | General")
	TSubclassOf<AC_BaseWeapon> BPRef;

	// The socket which the weapon will be attached to in first person
	UPROPERTY(EditDefaultsOnly, Category = "Base Weapon | Sockets")
	FName Socket1P;

	// The socket which the weapon will be attached to in third person
	UPROPERTY(EditDefaultsOnly, Category = "Base Weapon | Sockets")
	FName Socket3P;

	// The socket which the weapon will be attached to in third person when holstered
	UPROPERTY(EditDefaultsOnly, Category = "Base Weapon | Sockets")
	FName Socket3PHolstered;

	// Getters for 1P anim montages

	UAnimMontage* GetWeaponEquipMontage();

	UAnimMontage* GetWeaponReloadMontage();

	UAnimMontage* GetWeaponFireMontage();

	// Getters for 3P anim montages

	UAnimMontage* GetWeapon3PEquipMontage();

	UAnimMontage* GetWeapon3PReloadMontage();

	UAnimMontage* GetWeapon3PFireMontage();


	EWeaponType Type;

	UPROPERTY(EditDefaultsOnly, Category = "Base Weapon | General")
	UTexture2D* WeaponIcon;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Base Weapon | Animations | 1P")
	UAnimMontage* WeaponEquipAnimation;

	UPROPERTY(EditDefaultsOnly, Category = "Base Weapon | Animations | 1P")
	UAnimMontage* ReloadAnimation;

	UPROPERTY(EditDefaultsOnly, Category = "Base Weapon | Animations | 1P")
	UAnimMontage* FireAnimation;

	UPROPERTY(EditDefaultsOnly, Category = "Base Weapon | Animations | 3P")
	UAnimMontage* Equip3PMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Base Weapon | Animations | 3P")
	UAnimMontage* Reload3PMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Base Weapon | Animations | 3P")
	UAnimMontage* Fire3PMontage;

	class AC_PlayerCharacter* Player;

// ATTACKING

public:

	virtual void Attack();

	virtual void StopAttack();
};
