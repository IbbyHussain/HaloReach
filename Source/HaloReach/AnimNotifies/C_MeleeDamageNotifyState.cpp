// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/AnimNotifies/C_MeleeDamageNotifyState.h"
#include "C_MeleeDamageNotifyState.h"
#include "HaloReach/Player/C_PlayerCharacter.h"

void UC_MeleeDamageNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
}

void UC_MeleeDamageNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	AC_PlayerCharacter* Player = Cast<AC_PlayerCharacter>(MeshComp->GetAnimInstance()->TryGetPawnOwner());
	if(Player)
	{
		Player->MeleeAttack(MeshComp);
	}
}

void UC_MeleeDamageNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
}
