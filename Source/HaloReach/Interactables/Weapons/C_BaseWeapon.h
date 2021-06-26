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

	UAnimMontage* GetWeaponEquipMontage();

	EWeaponType Type;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Combat | Animations")
	UAnimMontage* WeaponEquipAnimation;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Animations")
	UAnimMontage* ReloadAnimation;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Animations")
	UAnimMontage* FireAnimation;

	class AC_PlayerCharacter* Player;

// ATTACKING

public:

	virtual void Attack();

	virtual void StopAttack();
};
