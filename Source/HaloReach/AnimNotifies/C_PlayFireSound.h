// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "C_PlayFireSound.generated.h"

UCLASS()
class HALOREACH_API UC_PlayFireSound : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	UPROPERTY(EditAnywhere, Category = "Sounds")
	USoundBase* FireSound;

	UPROPERTY(EditAnywhere, Category = "Sounds")
	USoundBase* DryFireSound;
	
};
