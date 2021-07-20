// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "C_DestroyActorNotify.generated.h"

UENUM(BlueprintType)
enum class EDestroyType : uint8
{
	DEFAULT,
	MAGAZINE,
	MAGAZINE3P

};

UCLASS()
class HALOREACH_API UC_DestroyActorNotify : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	
	UPROPERTY(EditAnywhere, Category = "Notify Params")
	EDestroyType DestroyType;
};
