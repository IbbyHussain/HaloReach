// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "C_SpawnActorNotify.generated.h"

/**
 * 
 */
UCLASS()
class HALOREACH_API UC_SpawnActorNotify : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	UPROPERTY(EditAnywhere, Category = "Notify Params")
	TSubclassOf<AActor> MagClass;

	UPROPERTY(EditAnywhere, Category = "Notify Params")
	FName SocketName;


	
};
