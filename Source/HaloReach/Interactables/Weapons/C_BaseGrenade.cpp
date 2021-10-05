// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/Interactables/Weapons/C_BaseGrenade.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "HaloReach/Player/C_PlayerCharacter.h"
#include "Camera/CameraComponent.h"

AC_BaseGrenade::AC_BaseGrenade()
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Comp"));
	RootComponent = MeshComp;

	RadialForceComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("Radial Force Comp"));
	RadialForceComp->SetupAttachment(RootComponent);
}

void AC_BaseGrenade::BeginPlay()
{
	Super::BeginPlay();

	MeshComp->SetSimulatePhysics(false);
	MeshComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
}

void AC_BaseGrenade::Thrown()
{
	MeshComp->SetSimulatePhysics(true);
	MeshComp->SetPhysicsLinearVelocity(FVector(0.0f));

	FVector CameraUpVector = Player->CameraComp->GetComponentRotation().Vector().UpVector;
	FVector CameraForwardVector = Player->CameraComp->GetComponentRotation().Vector().ForwardVector;

	FVector LaunchForce = (FMath::Lerp(CameraUpVector, CameraForwardVector, 1.0f)) * ThrowForce;

	MeshComp->AddImpulse(LaunchForce, NAME_None, true);
}



