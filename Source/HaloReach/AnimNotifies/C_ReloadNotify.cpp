// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/AnimNotifies/C_ReloadNotify.h"
#include "HaloReach/Player/C_PlayerCharacter.h"
#include "HaloReach/Interactables/Weapons/Guns/C_BaseGun.h"

void UC_ReloadNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	//Super::Notify(MeshComp, Animation);

	//GEngine->AddOnScreenDebugMessage(-1, 4.5f, FColor::Purple, __FUNCTION__);

	// Expensive replace will delegate
	AC_PlayerCharacter* Player = Cast<AC_PlayerCharacter>(MeshComp->GetAnimInstance()->TryGetPawnOwner());
	if(Player)
	{
		Player->UpdateReserveAmmo();

		AC_BaseGun* Gun = Cast<AC_BaseGun>(Player->EquippedWeaponArray[0]);
		if(Gun)
		{
			Gun->UpdateAmmoCounter();
		}
	}
}