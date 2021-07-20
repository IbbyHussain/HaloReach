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
		GEngine->AddOnScreenDebugMessage(-1, 4.5f, FColor::Purple, TEXT("Destroyed object using DEFAULT"));

	case EDestroyType::MAGAZINE:
		GEngine->AddOnScreenDebugMessage(-1, 4.5f, FColor::Purple, TEXT("Destroyed object using ATTACHED"));

		if (Player && Player->WeaponMag)
		{
			UC_SpawnLibrary::DestroyActor(Player->WeaponMag);
		}
	}
}