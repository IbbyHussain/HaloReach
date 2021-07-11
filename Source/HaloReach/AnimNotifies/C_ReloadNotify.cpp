// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/AnimNotifies/C_ReloadNotify.h"

void UC_ReloadNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	//Super::Notify(MeshComp, Animation);

	GEngine->AddOnScreenDebugMessage(-1, 4.5f, FColor::Purple, __FUNCTION__);
}