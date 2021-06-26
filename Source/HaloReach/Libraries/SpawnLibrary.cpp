// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/Libraries/SpawnLibrary.h"

AActor* USpawnLibrary::SpawnObject(AActor* Actor, TSubclassOf<AActor> ActorClass, USkeletalMeshComponent* Mesh, FName SocketName)
{
	//Spawn new weapon in first person
	FActorSpawnParameters SpawnParams;

	FTransform Transform = Mesh->GetSocketTransform(SocketName, ERelativeTransformSpace::RTS_World);
	FVector SpawnLocation = Transform.GetLocation();
	FRotator SpawnRotation = Transform.GetRotation().Rotator();

	return Actor = GetWorld()->SpawnActor<AActor>(ActorClass, SpawnLocation, SpawnRotation, SpawnParams);
}

AActor* USpawnLibrary::SpawnObjectAttached(AActor* Actor, TSubclassOf<AActor> ActorClass, USkeletalMeshComponent* Mesh, FName SocketName)
{
	//Spawn new weapon in first person
	FActorSpawnParameters SpawnParams;

	FTransform Transform = Mesh->GetSocketTransform(SocketName, ERelativeTransformSpace::RTS_World);
	FVector SpawnLocation = Transform.GetLocation();
	FRotator SpawnRotation = Transform.GetRotation().Rotator();

	Actor = GetWorld()->SpawnActor<AActor>(ActorClass, SpawnLocation, SpawnRotation, SpawnParams);
	Actor->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);

	return Actor;
}