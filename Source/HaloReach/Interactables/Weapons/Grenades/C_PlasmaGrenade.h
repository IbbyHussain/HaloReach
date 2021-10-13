// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HaloReach/Interactables/Weapons/C_BaseGrenade.h"
#include "C_PlasmaGrenade.generated.h"

/**
 * 
 */
UCLASS()
class HALOREACH_API AC_PlasmaGrenade : public AC_BaseGrenade
{
	GENERATED_BODY()

public:

	AC_PlasmaGrenade();

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Grenade")
	class UBoxComponent* BoxComp;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Only start on overlap begin event when the grenade has been thrown
	bool bHasGrenadeBeenThrown;

	// Ensure start explosion is only called once
	bool bStartExplosionOnce;

	virtual void Thrown(FVector ImpulseDirection) override;

	
};
