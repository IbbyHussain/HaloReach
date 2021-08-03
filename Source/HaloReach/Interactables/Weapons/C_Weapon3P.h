// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "C_Weapon3P.generated.h"

UCLASS()
class HALOREACH_API AC_Weapon3P : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AC_Weapon3P();

	UPROPERTY(EditDefaultsOnly, Category = "Pickup | Components")
	USkeletalMeshComponent* WeaponMesh3P;

};
