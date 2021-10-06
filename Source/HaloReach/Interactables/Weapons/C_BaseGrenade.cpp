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

	ThrowForce = 2000.0f;

	bReplicates = true;
	SetReplicateMovement(true);



}

void AC_BaseGrenade::BeginPlay()
{
	Super::BeginPlay();

	MeshComp->SetSimulatePhysics(false);
	MeshComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
}

void AC_BaseGrenade::Thrown(AC_PlayerCharacter* PlayerCharacter)
{
	/*if (PlayerCharacter)
	{
		MeshComp->SetSimulatePhysics(true);
		MeshComp->SetPhysicsLinearVelocity(FVector(0.0f, 0.0f, 0.0f), false, NAME_None);

		MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		MeshComp->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);

		FVector CameraUpVector = PlayerCharacter->CameraComp->GetUpVector();
		FVector CameraForwardVector = PlayerCharacter->CameraComp->GetForwardVector();

		FVector LaunchForce = (FMath::Lerp(CameraUpVector, CameraForwardVector, 1.0f)) * ThrowForce;

		MeshComp->AddImpulse(LaunchForce, NAME_None, true);
	}*/

	MeshComp->SetSimulatePhysics(true);
	MeshComp->SetPhysicsLinearVelocity(FVector(0.0f, 0.0f, 0.0f), false, NAME_None);
}

void AC_BaseGrenade::Server_Thrown_Implementation(AC_PlayerCharacter* PlayerCharacter)
{
	if (PlayerCharacter)
	{
		MeshComp->SetSimulatePhysics(true);
		MeshComp->SetPhysicsLinearVelocity(FVector(0.0f, 0.0f, 0.0f), false, NAME_None);

		MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		MeshComp->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);

		FVector CameraUpVector = PlayerCharacter->CameraComp->GetUpVector();
		FVector CameraForwardVector = PlayerCharacter->CameraComp->GetForwardVector();

		FVector LaunchForce = (FMath::Lerp(CameraUpVector, CameraForwardVector, 1.0f)) * ThrowForce;

		MeshComp->AddImpulse(LaunchForce, NAME_None, true);
	}
}



