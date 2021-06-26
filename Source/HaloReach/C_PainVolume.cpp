// Fill out your copyright notice in the Description page of Project Settings.


#include "C_PainVolume.h"
#include "HaloReach/Player/C_PlayerCharacter.h"


AC_PainVolume::AC_PainVolume()
{
 	
}

void AC_PainVolume::BeginPlay()
{
	Super::BeginPlay();
}


void AC_PainVolume::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

