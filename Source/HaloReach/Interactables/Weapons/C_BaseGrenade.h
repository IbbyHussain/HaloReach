// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "C_BaseGrenade.generated.h"

UCLASS()
class HALOREACH_API AC_BaseGrenade : public AActor
{
	GENERATED_BODY()
	
public:	

	AC_BaseGrenade();

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Grenade")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(EditDefaultsOnly, Category = "Grenade")
	class URadialForceComponent* RadialForceComp;

	// Enables physics on object after it has been thrown
	void Thrown();

	class AC_PlayerCharacter* Player;

	float ThrowForce;

	float Alpha;

};
