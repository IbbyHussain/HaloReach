// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HaloReach/Interfaces/C_InteractInterface.h"
#include "C_BasePickup.generated.h"

class AC_BaseWeapon;

UCLASS()
class HALOREACH_API AC_BasePickup : public AActor, public IC_InteractInterface
{
	GENERATED_BODY()
	
public:	
	AC_BasePickup();

	bool bIsOverlapped;

protected:

// VIRTUAL FUNCTIONS
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Interact();

	virtual void Interact_Implementation() override;

// CLASS COMPONENTS

	UPROPERTY(EditDefaultsOnly, Category = "Pickup | Components")
	USkeletalMeshComponent* PickupMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Pickup | Components")
	class UBoxComponent* BoxComp;

// INTERACT
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void OnOverlap();

	virtual void OnOverlapEnd();

	virtual void OnInteract();

// CLASS REFERENCES

	class AC_PlayerCharacter* Player;

};
