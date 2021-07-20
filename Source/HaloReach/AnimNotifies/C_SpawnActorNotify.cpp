// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/AnimNotifies/C_SpawnActorNotify.h"
#include "HaloReach/Player/C_PlayerCharacter.h"
#include "HaloReach/Interactables/Weapons/Guns/C_BaseGun.h"
#include "HaloReach/Libraries/C_SpawnLibrary.h"

void UC_SpawnActorNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	// Expensive replace will delegate
	AC_PlayerCharacter* Player = Cast<AC_PlayerCharacter>(MeshComp->GetAnimInstance()->TryGetPawnOwner());

	//GEngine->AddOnScreenDebugMessage(-1, 4.5f, FColor::Purple, __FUNCTION__);

	switch(SpawnType)
	{
	case ESpawnType::DEFAULT:
		GEngine->AddOnScreenDebugMessage(-1, 4.5f, FColor::Purple, TEXT("Spawned object using DEFAULT"));

	case ESpawnType::MAGAZINE:
		GEngine->AddOnScreenDebugMessage(-1, 4.5f, FColor::Purple, TEXT("Spawned object using ATTACHED"));

		if(Player)
		{
			//Player->BasicSpawnActor(MagClass);
			Player->WeaponMag = UC_SpawnLibrary::SpawnActorAtSocket<AActor>(Player->GetWorld(), ActorClass,
			Player->WeaponMag, Player->GetDefaultMesh(), Player->MagSocket);
		}
	}
}