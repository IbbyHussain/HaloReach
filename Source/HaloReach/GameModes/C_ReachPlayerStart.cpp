// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/GameModes/C_ReachPlayerStart.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "HaloReach/Player/C_PlayerCharacter.h"

AC_ReachPlayerStart::AC_ReachPlayerStart(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 500.0f;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
}

bool AC_ReachPlayerStart::bIsSuitable()
{
	const FVector StartLocation = GetActorLocation();
	const FVector EndLocation = GetActorLocation();

	TArray<AActor*> IgnoredActorArray;
	
	FHitResult Hit;

	ETraceTypeQuery Query =  UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Camera);

	const bool bHit = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), StartLocation, EndLocation, SphereRadius, 
		Query, false, IgnoredActorArray, EDrawDebugTrace::ForDuration, Hit, true, FLinearColor::Red, 
		FLinearColor::Blue, 5.0f);

	if(bHit)
	{
		AC_PlayerCharacter* PlayerCharacter = Cast<AC_PlayerCharacter>(Hit.GetActor());
		if(PlayerCharacter)
		{
			return false;
		}

		// what if collides with vechile or physics object?
		return false;
	}

	return true;
}