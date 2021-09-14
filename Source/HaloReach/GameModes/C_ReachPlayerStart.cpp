// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/GameModes/C_ReachPlayerStart.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

AC_ReachPlayerStart::AC_ReachPlayerStart(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 500.0f;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
}