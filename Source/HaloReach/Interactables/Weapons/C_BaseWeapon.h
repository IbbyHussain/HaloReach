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

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<AC_BaseWeapon> BPRef;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName Socket1P;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName Socket3P;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,  Category = "Weapon")
	UTexture2D* WeaponIcon;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Animations | 1P")
	UAnimMontage* WeaponEquipAnimation;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Animations | 1P")
	UAnimMontage* ReloadAnimation;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Animations | 1P")
	UAnimMontage* FireAnimation;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Animations | 3P")
	UAnimMontage* Equip3PMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Animations | 3P")
	UAnimMontage* Reload3PMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Animations | 3P")
	UAnimMontage* Fire3PMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	class AC_PlayerCharacter* Player;

// ATTACKING

public:

	virtual void Attack();

	virtual void StopAttack();
};
