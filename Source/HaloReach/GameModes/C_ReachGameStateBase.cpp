// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/GameModes/C_ReachGameStateBase.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetMathLibrary.h"
#include "HaloReach/UI/HUD/C_PlayerHUD.h"


AC_ReachGameStateBase::AC_ReachGameStateBase()
{
}

FString AC_ReachGameStateBase::GetPlayersName()
{
	int RandomIndex = UKismetMathLibrary::RandomInteger(PlayerNamesArray.Num());
	FString NewPlayerName = PlayerNamesArray[RandomIndex];
	return NewPlayerName;
}
