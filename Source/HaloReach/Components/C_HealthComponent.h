// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "C_HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnHealthChangedSignature, UC_HealthComponent*, HealthComponent, float, Health, bool, bUpdateCombatState, AActor*, PlayerKiller);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnShieldsChangedSignature, UC_HealthComponent*, HealthComponent, float, Shields);

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HALOREACH_API UC_HealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UC_HealthComponent();

// DELEGATES

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealthChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnShieldsChangedSignature OnShieldsChanged;

// EXPOSED MEMBERS

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Health Component")
	float MaxHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Health Component")
	float MaxShields;

// REGENERATION

	void StartShieldRegeneration();

	void StopShieldRegeneration();

	void StartHealthRegeneration();

	void StopHealthRegeneration();

// GETTER METHODS

	UFUNCTION(BlueprintCallable) // TEMP
	float GetHealth();

	UFUNCTION(BlueprintCallable) // TEMP
	float GetShields();

private:

	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UPROPERTY(Replicated)
	float Shields;

	UPROPERTY(Replicated)
	float Health;

	// Shield Regeneration

	void RechargeShields();

	FTimerHandle ShieldRegenHandle;

	// Health Regeneration

	void RechargeHealth();

	FTimerHandle HealthRegenHandle;

// REPLICATION

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// A client RPC used to replicate the HandleTakeAnyDamage bidning of OnTakeAnyDamage, to the client as the function is server only
	UFUNCTION(Client, Reliable)
	void Client_Test(float Damage, AActor* Killer);
	void Client_Test_Implementation(float Damage, AActor* Killer);


	// fffafa

	UFUNCTION(Server, Reliable)
	void Server_Test(float Damage, AActor* Killer);
	void Server_Test_Implementation(float Damage, AActor* Killer);

	UPROPERTY()
	FString GlobalKillerName;

	UFUNCTION(Client, Reliable)
	void Client_Name(AActor* Killer);
	void Client_Name_Implementation(AActor* Killer);

	AActor* LocalKiller;

public:
	UFUNCTION(BlueprintImplementableEvent)
	void OnTakeDamageBP();
};

