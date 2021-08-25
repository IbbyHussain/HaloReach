// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "C_SpawnLibrary.generated.h"

/**
 * 
 */
UCLASS()
class HALOREACH_API UC_SpawnLibrary : public UObject
{
	GENERATED_BODY()

public:

	UC_SpawnLibrary(const FObjectInitializer& ObjectInitializer);

	// World ptr as static methods cannot call non static methods so need to static cast world to get spawnactor function
	template<typename ClassName>
	static ClassName* SpawnActorAtSocket(UWorld* World, TSubclassOf<ClassName> ObjectClass, ClassName* ObjectPointer, USkeletalMeshComponent* MeshComp, FName SpawnSocket);

	template<typename ClassName>
	static ClassName* SpawnActorAtLocation(UWorld* World, TSubclassOf<ClassName> ObjectClass, ClassName* ObjectPointer, FVector Location, FRotator Rotation);

	static void DestroyActor(AActor* ActorToDestroy);
};

template<typename ClassName>
ClassName* UC_SpawnLibrary::SpawnActorAtSocket(UWorld* World, TSubclassOf<ClassName> ObjectClass, ClassName* ObjectPointer, USkeletalMeshComponent* MeshComp, FName SpawnSocket)
{
	//Simple spawn actor attached to a socket
	FActorSpawnParameters SpawnParams;

	FTransform Transform = MeshComp->GetSocketTransform(SpawnSocket, ERelativeTransformSpace::RTS_World);
	FVector SpawnLocation = Transform.GetLocation();
	FRotator SpawnRotation = Transform.GetRotation().Rotator();

	// Static cast to get world and call non-static functions
	auto NewWorld = static_cast<UWorld*>(World);

	ObjectPointer = World->SpawnActor<ClassName>(ObjectClass, SpawnLocation, SpawnRotation, SpawnParams);
	ObjectPointer->AttachToComponent(MeshComp, FAttachmentTransformRules::SnapToTargetIncludingScale, SpawnSocket);

	return ObjectPointer;
}

template<typename ClassName>
ClassName* UC_SpawnLibrary::SpawnActorAtLocation(UWorld* World, TSubclassOf<ClassName> ObjectClass, ClassName* ObjectPointer, FVector Location, FRotator Rotation)
{
	//Simple spawn actor attached to a socket
	FActorSpawnParameters SpawnParams;

	// Static cast to get world and call non-static functions
	auto NewWorld = static_cast<UWorld*>(World);

	ObjectPointer = World->SpawnActor<ClassName>(ObjectClass, Location, Rotation, SpawnParams);

	return ObjectPointer;
}