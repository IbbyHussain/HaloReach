// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/AnimNotifies/C_DestroyActorNotify.h"
#include "HaloReach/Player/C_PlayerCharacter.h"
#include "HaloReach/Interactables/Weapons/Guns/C_BaseGun.h"
#include "HaloReach/Libraries/C_SpawnLibrary.h"


void UC_DestroyActorNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	//GEngine->AddOnScreenDebugMessage(-1, 4.5f, FColor::Purple, __FUNCTION__);

	// Expensive replace will delegate
	AC_PlayerCharacter* Player = Cast<AC_PlayerCharacter>(MeshComp->GetAnimInstance()->TryGetPawnOwner());

	switch (DestroyType)
	{
	case EDestroyType::DEFAULT:

	case EDestroyType::MAGAZINE:

		if (Player && Player->Weapon1PMag)
		{
			UC_SpawnLibrary::DestroyActor(Player->Weapon1PMag);
		}
		break;

	case EDestroyType::MAGAZINE3P:

		if (Player && Player->Weapon3PMag)
		{
			UC_SpawnLibrary::DestroyActor(Player->Weapon3PMag);
		}
		break;
	}
}
