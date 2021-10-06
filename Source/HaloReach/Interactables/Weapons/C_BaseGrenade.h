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
	void Thrown(AC_PlayerCharacter* PlayerCharacter);

	UFUNCTION(Server, Reliable)
	void Server_Thrown(AC_PlayerCharacter* PlayerCharacter);
	void Server_Thrown_Implementation(AC_PlayerCharacter* PlayerCharacter);


	UPROPERTY(EditDefaultsOnly, Category = "Grenade")
	float ThrowForce;
};
