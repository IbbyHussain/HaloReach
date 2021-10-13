// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/Interactables/Weapons/Grenades/C_FragGrenade.h"

AC_FragGrenade::AC_FragGrenade()
{

}

void AC_FragGrenade::StartExplosion()
{
	Super::StartExplosion();

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Frag Start Exploded")));
}

void AC_FragGrenade::Thrown(FVector ImpulseDirection)
{
	Super::Thrown(ImpulseDirection);

	StartExplosion();

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Frag Start THROWN Exploded")));
}
