// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/GameModes/Slayer/C_SlayerGameStateBase.h"

AC_SlayerGameStateBase::AC_SlayerGameStateBase()
{
	PrimaryActorTick.bCanEverTick = true;

	GameModeName = "Slayer";

	MatchLength = 10;

	MatchMinutes = 1;
	MatchSeconds = 10;
}