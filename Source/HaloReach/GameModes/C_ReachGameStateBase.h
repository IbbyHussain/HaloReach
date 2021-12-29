// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "C_ReachGameStateBase.generated.h"

class AC_PlayerCharacter;
class AC_ReachPlayerStart;
 
UCLASS()
class HALOREACH_API AC_ReachGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
public:

	AC_ReachGameStateBase();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void UpdateGlobalDeaths(FString A, FString B);

	TArray<AC_ReachPlayerStart*> PlayerStartArray;

	// GameMode attributes

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GM Attributes")
	FString GameModeName;

	FString GetGMName() const { return GameModeName; }

	UPROPERTY(Replicated)
	int MatchMinutes;

	UPROPERTY(Replicated)
	int MatchSeconds;

	UFUNCTION(BlueprintCallable)
	int GetMatchMinutes() const { return MatchMinutes; }

	UFUNCTION(BlueprintCallable)
	int GetMatchSeconds() const { return MatchSeconds; }

	void SetMatchMinutes(int NewMatchMinutes)
	{
		MatchMinutes = NewMatchMinutes;
	}

	void SetMatchSeconds(int NewMatchSeconds)
	{
		MatchSeconds = NewMatchSeconds;
	}

	UPROPERTY(ReplicatedUsing = OnRep_GameOver)
	bool bGameOver;

	UFUNCTION()
	void OnRep_GameOver();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Client, Reliable)
	void a();
	void a_Implementation();

	FTimerHandle DestroyGameHandle;

	UFUNCTION(BlueprintImplementableEvent)
	void DestroyGameSession();
};
