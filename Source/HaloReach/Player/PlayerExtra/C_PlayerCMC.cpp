// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/Player/PlayerExtra/C_PlayerCMC.h"
#include "GameFramework/Character.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"

UC_PlayerCMC::UC_PlayerCMC()
{

}

void UC_PlayerCMC::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UC_PlayerCMC::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	/*  There are 4 custom move flags for us to use. Below is what each is currently being used for:
		FLAG_Custom_0		= 0x10, // Sprinting
		FLAG_Custom_1		= 0x20, // WallRunning
		FLAG_Custom_2		= 0x40, // Unused
		FLAG_Custom_3		= 0x80, // Unused
	*/

	// Read the values from the compressed flags, sent to the server
	WantsToSprint = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;
	WallRunKeysDown = (Flags & FSavedMove_Character::FLAG_Custom_1) != 0;
}

FNetworkPredictionData_Client* UC_PlayerCMC::GetPredictionData_Client() const
{
	if (ClientPredictionData == nullptr)
	{
		// Return our custom client prediction class instead
		UC_PlayerCMC* MutableThis = const_cast<UC_PlayerCMC*>(this);
		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_My(*this);
	}

	return ClientPredictionData;
}

#pragma region Saved Move


void FSavedMove_My::Clear()
{
	Super::Clear();

	// Clear all values
	SavedWantsToSprint = 0;
	SavedWallRunKeysDown = 0;
}

uint8 FSavedMove_My::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();

	/* There are 4 custom move flags for us to use. Below is what each is currently being used for:
	FLAG_Custom_0		= 0x10, // Sprinting
	FLAG_Custom_1		= 0x20, // WallRunning
	FLAG_Custom_2		= 0x40, // Unused
	FLAG_Custom_3		= 0x80, // Unused
	*/

	// Write to the compressed flags, copy data to the compressed flags
	if (SavedWantsToSprint)
		Result |= FLAG_Custom_0;
	if (SavedWallRunKeysDown)
		Result |= FLAG_Custom_1;

	

	return Result;
}

bool FSavedMove_My::CanCombineWith(const FSavedMovePtr& NewMovePtr, ACharacter* Character, float MaxDelta) const
{
	const FSavedMove_My* NewMove = static_cast<const FSavedMove_My*>(NewMovePtr.Get());

	// As an optimization, check if the engine can combine saved moves.
	if (SavedWantsToSprint != NewMove->SavedWantsToSprint ||
		SavedWallRunKeysDown != NewMove->SavedWallRunKeysDown)
	{
		return false;
	}

	return Super::CanCombineWith(NewMovePtr, Character, MaxDelta);
}

void FSavedMove_My::SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
	Super::SetMoveFor(Character, InDeltaTime, NewAccel, ClientData);

	UC_PlayerCMC* charMov = static_cast<UC_PlayerCMC*>(Character->GetCharacterMovement());
	if (charMov)
	{
		// Copy values into the saved move
		SavedWantsToSprint = charMov->WantsToSprint;
		SavedWallRunKeysDown = charMov->WallRunKeysDown;
	}
}

void FSavedMove_My::PrepMoveFor(ACharacter* Character)
{
	Super::PrepMoveFor(Character);

	UC_PlayerCMC* charMov = Cast<UC_PlayerCMC>(Character->GetCharacterMovement());
	if (charMov)
	{
		// Copy values out of the saved move
		charMov->WantsToSprint = SavedWantsToSprint;
		charMov->WallRunKeysDown = SavedWallRunKeysDown;
	}
}


#pragma endregion 

# pragma region Network Prediction Data


FNetworkPredictionData_Client_My::FNetworkPredictionData_Client_My(const UCharacterMovementComponent& ClientMovement) : Super(ClientMovement)
{

}

FSavedMovePtr FNetworkPredictionData_Client_My::AllocateNewMove()
{
	return FSavedMovePtr(new FSavedMove_My());
}


#pragma endregion

