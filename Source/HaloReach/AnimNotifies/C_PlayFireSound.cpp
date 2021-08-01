// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/AnimNotifies/C_PlayFireSound.h"

#include "HaloReach/Player/C_PlayerCharacter.h"
#include "HaloReach/Interactables/Weapons/Guns/C_BaseGun.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

void UC_PlayFireSound::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	//Super::Notify(MeshComp, Animation);

	//GEngine->AddOnScreenDebugMessage(-1, 4.5f, FColor::Purple, __FUNCTION__);

	// Expensive replace will delegate
	AC_PlayerCharacter* Player = Cast<AC_PlayerCharacter>(MeshComp->GetAnimInstance()->TryGetPawnOwner());
	if (Player)
	{
		AC_BaseGun* Gun = Cast<AC_BaseGun>(Player->EquippedWeaponArray[0]);
		if (Gun->WeaponStats.CurrentAmmo != 0)
		{
			UGameplayStatics::PlaySound2D(Player->GetWorld(), FireSound);
			//UGameplayStatics::PlaySoundAtLocation(GetWorld(), DryFireSound, Player->GetActorLocation());
		}

		else
		{
			UGameplayStatics::PlaySound2D(Player->GetWorld(), DryFireSound);
			//UGameplayStatics::PlaySoundAtLocation(GetWorld(), FireSound, Player->GetActorLocation());
		}

	}
}