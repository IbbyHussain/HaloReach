// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/Interactables/Weapons/C_BaseGrenade.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "HaloReach/Player/C_PlayerCharacter.h"
#include "Camera/CameraComponent.h"

AC_BaseGrenade::AC_BaseGrenade()
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Comp"));
	MeshComp->SetOnlyOwnerSee(true);
	RootComponent = MeshComp;

	RadialForceComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("Radial Force Comp"));
	RadialForceComp->SetupAttachment(RootComponent);

	bReplicates = true;
	SetReplicateMovement(true);


}

void AC_BaseGrenade::BeginPlay()
{
	Super::BeginPlay();

	MeshComp->SetSimulatePhysics(false);
	MeshComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
}

void AC_BaseGrenade::Thrown(FVector ImpulseDirection)
{
	
	MeshComp->SetSimulatePhysics(true);
	MeshComp->SetPhysicsLinearVelocity(FVector(0.0f, 0.0f, 0.0f), false, NAME_None);

	MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComp->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);

	MeshComp->AddImpulse(ImpulseDirection, NAME_None, true);
}

void AC_BaseGrenade::Multi_bSetOnlyOwnerSeeMesh_Implementation(bool bCanSee)
{
	MeshComp->SetOnlyOwnerSee(bCanSee);
}


