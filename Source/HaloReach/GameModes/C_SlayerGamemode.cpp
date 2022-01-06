// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/GameModes/C_SlayerGamemode.h"
#include "HaloReach/Player/PlayerExtra/C_ReachPlayerState.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetArrayLibrary.h"
#include "HaloReach/GlobalEnums.h"

AC_SlayerGamemode::AC_SlayerGamemode()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AC_SlayerGamemode::BeginPlay()
{
	Super::BeginPlay();
}

void AC_SlayerGamemode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AC_SlayerGamemode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
}

void AC_SlayerGamemode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
}




