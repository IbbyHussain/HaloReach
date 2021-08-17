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

	//Set Max Walk Speed
	if(WantsToChangeSpeed)
	{
		WantsToChangeSpeed = false;
		MaxWalkSpeed = MyNewMaxWalkSpeed;
	}
}

void UC_PlayerCMC::Server_SetMaxWalkSpeed_Implementation(const float NewMaxWalkSpeed)
{
	MyNewMaxWalkSpeed = NewMaxWalkSpeed;
}

void UC_PlayerCMC::SetMaxWalkSpeed(float NewMaxWalkSpeed)
{
	if (PawnOwner->IsLocallyControlled())
	{
		MyNewMaxWalkSpeed = NewMaxWalkSpeed;
		Server_SetMaxWalkSpeed(NewMaxWalkSpeed);
	}

	WantsToChangeSpeed = true;
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
	WantsToChangeSpeed = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;
}

//float UC_PlayerCMC::GetMaxSpeed() const
//{
//	switch (MovementMode)
//	{
//	case MOVE_Walking:
//	case MOVE_NavWalking:
//	{
//		/*if (IsCrouching())
//		{
//			return MaxWalkSpeedCrouched;
//		}
//
//		else
//		{
//			if (WantsToSprint)
//			{
//				return SprintSpeed;
//			}
//
//			return DefaultSpeed;
//		}*/
//	}
//	case MOVE_Falling:
//		return DefaultSpeed;
//	case MOVE_Swimming:
//		return MaxSwimSpeed;
//	case MOVE_Flying:
//		return MaxFlySpeed;
//	case MOVE_Custom:
//		return MaxCustomMovementSpeed;
//	case MOVE_None:
//	default:
//		return 0.f;
//	}
//}

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

void UC_PlayerCMC::SetSprinting(bool sprinting)
{
	//bWantsToS
}

#pragma region Saved Move


void FSavedMove_My::Clear()
{
	Super::Clear();

	// Clear all values
	SavedWantsToChangeSpeed = 0;
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
	if (SavedWantsToChangeSpeed)
	{
		Result |= FLAG_Custom_0;
	}

	return Result;
}

bool FSavedMove_My::CanCombineWith(const FSavedMovePtr& NewMovePtr, ACharacter* Character, float MaxDelta) const
{
	//const FSavedMove_My* NewMove = static_cast<const FSavedMove_My*>(NewMovePtr.Get());

	// As an optimization, check if the engine can combine saved moves.
	/*if (SavedWantsToSprint != NewMove->SavedWantsToSprint ||
		SavedWallRunKeysDown != NewMove->SavedWallRunKeysDown)
	{
		return false;
	}*/

	if (SavedWantsToChangeSpeed != ((FSavedMove_My*)&NewMovePtr)->SavedWantsToChangeSpeed)
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
		SavedWantsToChangeSpeed = charMov->WantsToChangeSpeed;
	}
}

void FSavedMove_My::PrepMoveFor(ACharacter* Character)
{
	Super::PrepMoveFor(Character);

	UC_PlayerCMC* charMov = Cast<UC_PlayerCMC>(Character->GetCharacterMovement());
	if (charMov)
	{
		// Copy values out of the saved move
		charMov->WantsToChangeSpeed = SavedWantsToChangeSpeed;
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

