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
	UFUNCTION(BlueprintCallable)
	void Thrown(FVector ImpulseDirection);

	UFUNCTION(NetMulticast, Reliable)
	void Multi_bSetOnlyOwnerSeeMesh(bool bCanSee);
	void Multi_bSetOnlyOwnerSeeMesh_Implementation(bool bCanSee);

	UPROPERTY(EditDefaultsOnly, Category = "Grenade")
	UParticleSystem* ExplosionEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Grenade")
	USoundBase* ExplosionSound;

	FTimerHandle ExplosionHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Grenade")
	float ExplosionDelay;

	// Virtual Functions 

	virtual void StartExplosion();

	virtual void Explode();



};
