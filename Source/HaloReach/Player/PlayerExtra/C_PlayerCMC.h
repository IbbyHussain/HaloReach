// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "C_PlayerCMC.generated.h"

/**
 * 
 */
UCLASS()
class HALOREACH_API UC_PlayerCMC : public UCharacterMovementComponent
{
	GENERATED_BODY()

	friend class FSavedMove_My;

public:

	UC_PlayerCMC();

	//Set Max Walk Speed

	UFUNCTION(Unreliable, Server)
	void Server_SetMaxWalkSpeed(const float NewMaxWalkSpeed);
	void Server_SetMaxWalkSpeed_Implementation(const float NewMaxWalkSpeed);

	float MyNewMaxWalkSpeed;

	//Set Max Walk Speed (Called from the owning client)
	UFUNCTION(BlueprintCallable, Category = "Max Walk Speed")
	void SetMaxWalkSpeed(float NewMaxWalkSpeed);

#pragma region Overrides
public:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
	void OnMovementUpdated(float DeltaTime, const FVector& OldLocation, const FVector& OldVelocity);
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;
#pragma endregion

#pragma region Compressed Flags

	// These compressed flags are copied into the FSavedMove_My classes compressed flags which are then sent to the server
	// bit flags, which act as booleans. Each custom movement mode needs one
private:
	uint8 WantsToSprint : 1;
	uint8 WallRunKeysDown : 1;
#pragma endregion

#pragma region Private Variables
	// True if the sprint key is down, used to control sprint movement
	bool SprintKeyDown = false;
#pragma endregion
};

class FSavedMove_My : public FSavedMove_Character
{
public:

	typedef FSavedMove_Character Super;

	// Resets all saved variables.
	virtual void Clear() override;

	// Store input commands in the compressed flags.
	virtual uint8 GetCompressedFlags() const override;

	// This is used to check whether or not two moves can be combined into one.
	// Basically you just check to make sure that the saved variables are the same.
	virtual bool CanCombineWith(const FSavedMovePtr& NewMovePtr, ACharacter* Character, float MaxDelta) const override;

	// Sets up the move before sending it to the server. 
	virtual void SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character& ClientData) override;

	// Sets variables on character movement component before making a predictive correction.
	virtual void PrepMoveFor(class ACharacter* Character) override;

private:

	// Compressed flags, basically booleans keeps client and server synched. Used by the server
	// Need one for each custom move
	uint8 SavedWantsToSprint : 1;
	uint8 SavedWallRunKeysDown : 1;
	
};

class FNetworkPredictionData_Client_My : public FNetworkPredictionData_Client_Character
{
public:
	typedef FNetworkPredictionData_Client_Character Super;

	// Constructor
	FNetworkPredictionData_Client_My(const UCharacterMovementComponent& ClientMovement);

	//brief Allocates a new copy of our custom saved move
	virtual FSavedMovePtr AllocateNewMove() override;
};