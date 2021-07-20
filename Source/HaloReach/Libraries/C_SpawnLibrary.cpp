// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/Libraries/C_SpawnLibrary.h"

UC_SpawnLibrary::UC_SpawnLibrary(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
}

void UC_SpawnLibrary::DestroyActor(AActor* ActorToDestroy)
{
	if(ActorToDestroy)
	{
		ActorToDestroy->Destroy();
	}
}