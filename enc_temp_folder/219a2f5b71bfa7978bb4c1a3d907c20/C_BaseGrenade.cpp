// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/Interactables/Weapons/C_BaseGrenade.h"

// Sets default values
AC_BaseGrenade::AC_BaseGrenade()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AC_BaseGrenade::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AC_BaseGrenade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

