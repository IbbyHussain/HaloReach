// Fill out your copyright notice in the Description page of Project Settings.


#include "C_PlayerCameraManager.h"

AC_PlayerCameraManager::AC_PlayerCameraManager()
{
}

void AC_PlayerCameraManager::SetViewPitchAmount(float MinPitch, float MaxPitch)
{
	ViewPitchMin = MinPitch;
	ViewPitchMax = MaxPitch;
}

