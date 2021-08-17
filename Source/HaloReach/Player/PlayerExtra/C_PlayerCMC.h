// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "C_PlayerCMC.generated.h"

UCLASS()
class HALOREACH_API UC_PlayerCMC : public UCharacterMovementComponent
{
	GENERATED_BODY()

	friend class FSavedMove_My;

public:

	UC_PlayerCMC();

#pragma region Player Speed

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "My Character Movement|Grounded", Meta = (AllowPrivateAccess = "true"))
	float SprintSpeed = 800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "My Character Movement|Grounded", Meta = (AllowPrivateAccess = "true"))
	float DefaultSpeed = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "My Character Movement|Grounded", Meta = (AllowPrivateAccess = "true"))
	float CrouchSpeed = 200.0f;

	UFUNCTION(Unreliable, Server)
	void Server_SetMaxWalkSpeed(const float NewMaxWalkSpeed);
	void Server_SetMaxWalkSpeed_Implementation(const float NewMaxWalkSpeed);

	float MyNewMaxWalkSpeed;

	UFUNCTION(BlueprintCallable, Category = "Max Walk Speed")
	void SetMaxWalkSpeed(float NewMaxWalkSpeed);

#pragma endregion

#pragma region Overrides
public:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
	//virtual float GetMaxSpeed() const override;
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;
#pragma endregion

#pragma region Compressed Flags

	// These compressed flags are copied into the FSavedMove_My classes compressed flags which are then sent to the server
	// bit flags, which act as booleans. Each custom movement mode needs one
private:

	// Flag for changing the max walk speed
	uint8 WantsToChangeSpeed : 1;
	uint8 WantsToDoge : 1;
	uint8 WantsToJetpack : 1;

#pragma endregion

#pragma region Private Variables

	// True if the sprint key is down, used to control sprint movement
	bool SprintKeyDown = false;
	bool DogeKeyDown = false;
	bool JetpackKeyDown = false;

#pragma endregion

	// Sets sprinting to either enabled or disabled, the function that controls sprinting
	UFUNCTION(BlueprintCallable, Category = "My Character Movement")
	void SetSprinting(bool sprinting);
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
	uint8 SavedWantsToChangeSpeed : 1;
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