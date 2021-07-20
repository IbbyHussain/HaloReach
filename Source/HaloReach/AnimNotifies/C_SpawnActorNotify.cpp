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

	case ESpawnType::MAGAZINE:

		if(Player)
		{
			//Player->BasicSpawnActor(MagClass);
			Player->Weapon1PMag = UC_SpawnLibrary::SpawnActorAtSocket<AActor>(Player->GetWorld(), ActorClass,
				Player->Weapon1PMag, Player->GetDefaultMesh(), Socket);

			Player->Weapon1PMag->SetOwner(Player);
		}
		break;

	case ESpawnType::MAGAZINE3P:

		if (Player)
		{
			//Player->BasicSpawnActor(MagClass);
			Player->Weapon3PMag = UC_SpawnLibrary::SpawnActorAtSocket<AActor>(Player->GetWorld(), ActorClass,
				Player->Weapon3PMag, Player->GetMesh3P(), Socket);

			Player->Weapon3PMag->SetOwner(Player);
		}
		break;
	}
}