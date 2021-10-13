// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/Interactables/Weapons/C_BaseGrenade.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "HaloReach/Player/C_PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "HaloReach/HaloReach.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "HaloReach/Components/C_HealthComponent.h"

#include "DrawDebugHelpers.h"

AC_BaseGrenade::AC_BaseGrenade()
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Comp"));
	MeshComp->SetOnlyOwnerSee(true);
	RootComponent = MeshComp;

	RadialForceComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("Radial Force Comp"));
	RadialForceComp->SetupAttachment(RootComponent);

	ExplosionDelay = 3.0f;

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

	// wil call a count down 
	StartExplosion();
}

void AC_BaseGrenade::Multi_bSetOnlyOwnerSeeMesh_Implementation(bool bCanSee)
{
	MeshComp->SetOnlyOwnerSee(bCanSee);
}

void AC_BaseGrenade::ClearIgnoredActorsArray()
{
	IgnoredActorsArray.Empty();
}

void AC_BaseGrenade::StartExplosion()
{
	ClearIgnoredActorsArray();
	GetWorldTimerManager().SetTimer(ExplosionHandle, this, &AC_BaseGrenade::Explode, ExplosionDelay, false);
}

void AC_BaseGrenade::Explode()
{
	// Alternative method: Use Apply radial damage

	FCollisionQueryParams CollisionParams;

	TArray<FHitResult> OutHits;

	FVector ActorLocation = GetActorLocation();

	// Creates a sphere
	FCollisionShape MyColSphere = FCollisionShape::MakeSphere(250.0f);

	// Debug 
	DrawDebugSphere(GetWorld(), ActorLocation, MyColSphere.GetSphereRadius(), 20, FColor::Green, true);

	// A sweep trace that will hit anything within the sphere
	bool bHit = GetWorld()->SweepMultiByChannel(OutHits, ActorLocation, ActorLocation, FQuat::Identity, COLLISION_MELEEDAMAGE, MyColSphere, CollisionParams);

	// fix - can be hit multiple times
	if (bHit)
	{
		for (auto& Hit : OutHits)
		{
			AC_PlayerCharacter* PlayerCharacter = Cast<AC_PlayerCharacter>(Hit.GetActor());

			if (PlayerCharacter && !IgnoredActorsArray.Contains(PlayerCharacter))
			{
				// Apply damage to player, Hack to get around main dmg system where dmg stops after shields are broken
				UGameplayStatics::ApplyDamage(PlayerCharacter, 100.0f, GetOwner()->GetInstigatorController(), GetOwner(), nullptr);
				UGameplayStatics::ApplyDamage(PlayerCharacter, 100.0f, GetOwner()->GetInstigatorController(), GetOwner(), nullptr);

				IgnoredActorsArray.Emplace(PlayerCharacter);
			}
		}
	}

	// play sound and effect

	FTransform ExplosionTransform;
	ExplosionTransform.SetLocation(GetActorLocation());
	ExplosionTransform.SetRotation(GetActorRotation().Quaternion());
	ExplosionTransform.SetScale3D(FVector(1.0f));

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, ExplosionTransform);

	UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionSound, GetActorLocation());

	Destroy();
}


