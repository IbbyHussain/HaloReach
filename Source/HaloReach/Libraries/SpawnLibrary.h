// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SpawnLibrary.generated.h"

/**
 * 
 */
UCLASS()
class HALOREACH_API USpawnLibrary : public UObject
{
	GENERATED_BODY()
	
public:

	AActor* SpawnObject(AActor* Actor, TSubclassOf<AActor> ActorClass, USkeletalMeshComponent* Mesh , FName SocketName);

	AActor* SpawnObjectAttached(AActor* Actor, TSubclassOf<AActor> ActorClass, USkeletalMeshComponent* Mesh, FName SocketName);
};
