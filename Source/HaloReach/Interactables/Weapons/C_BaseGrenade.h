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

	UFUNCTION(NetMulticast, Reliable)
	void Multi_bSetOnlyOwnerSeeMesh(bool bCanSee);
	void Multi_bSetOnlyOwnerSeeMesh_Implementation(bool bCanSee);

	FTimerHandle ExplosionHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Grenade")
	float ExplosionDelay;

	TArray<AActor*> IgnoredActorsArray;

	void ClearIgnoredActorsArray();

# pragma region Grenade Effects

	UPROPERTY(EditDefaultsOnly, Category = "Grenade")
	UParticleSystem* ExplosionEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Grenade")
	USoundBase* ExplosionSound;

	UFUNCTION(Server, Reliable)
	void Server_SpawnEffects(FTransform EffectTransform);
	void Server_SpawnEffects_Implementation(FTransform EffectTransform);

	UFUNCTION(NetMulticast, Reliable)
	void Multi_SpawnEffects(FTransform EffectTransform);
	void Multi_SpawnEffects_Implementation(FTransform EffectTransform);


# pragma endregion

	// Virtual Functions 

	virtual void StartExplosion();

	virtual void Explode();

	// Enables physics on object after it has been thrown
	UFUNCTION(BlueprintCallable)
	virtual void Thrown(FVector ImpulseDirection);



};
