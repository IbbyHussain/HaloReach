
#include "HaloReach/Interactables/Weapons/Guns/C_BaseGun.h"
#include "DrawDebugHelpers.h"
#include "HaloReach/Player/C_PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Camera/CameraShake.h"
#include "HaloReach/HaloReach.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"

AC_BaseGun::AC_BaseGun()
{
	BaseDamage = 15.0f;

	// Rounds per minute
	RateOfFire = 600.0f;

	bReplicates = true;

	NetUpdateFrequency = 30.0f;
	MinNetUpdateFrequency = 10.0f;
}

void AC_BaseGun::BeginPlay()
{
	Super::BeginPlay();

	// Divide by 60 to get per minute
	TimeBetweenShots = 60.0f / RateOfFire;
}

// Overriden from base class
void AC_BaseGun::Attack()
{
	StartFire();
}

void AC_BaseGun::StopAttack()
{
	//	Super::Attack();
	StopFire();
}

void AC_BaseGun::Fire()
{
	AActor* MyOwner = GetOwner();

	if(!(MyOwner->HasAuthority()))
	{
		Server_Fire();
		//UE_LOG(LogTemp, Log, TEXT("CLIENT CALLED SERVER_FIRE()"));

		//return;
	}

	if(MyOwner)
	{
		//UE_LOG(LogTemp, Log, TEXT("SERVER FIRED"));

		FVector EyeLocation;
		FRotator EyeRotation;
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector ShotDirection = EyeRotation.Vector();
		FVector TraceEnd = EyeLocation + (ShotDirection * 5000);

		FCollisionQueryParams QueryParams;

		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;
		QueryParams.bReturnPhysicalMaterial = true;

		FHitResult Hit;

		bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryParams);

		if(bHit)
		{
			AActor* HitActor = Hit.GetActor();

			EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

			// Head shot damage should only be applied when shields = 0

			float ActualDamage = BaseDamage;
			if (SurfaceType == SURFACE_FLESHVULNERABLE)
			{
				ActualDamage *= 3.0f;
			}

			UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, Hit, MyOwner->GetInstigatorController(), this, DamageType);

			DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::Red, false, 1.0f, 0, 1.0f);

			UParticleSystem* SelectedEffect = nullptr;
			switch (SurfaceType)
			{
			case SURFACE_FLESHDEFAULT:
			case SURFACE_FLESHVULNERABLE:
				SelectedEffect = FleshImpactEffect;
				break;

			default:
				SelectedEffect = DefaultImpactEffect;
				break;
			}

			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
		}

		PlayFireEffects();

		LastFireTime = GetWorld()->TimeSeconds;
	}
}

void AC_BaseGun::PlayFireEffects()
{
	UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, WeaponMesh, MuzzleSocketName);

	APawn* MyOwner = Cast<APawn>(GetOwner());

	if(MyOwner)
	{
		APlayerController* PC = Cast<APlayerController>(MyOwner->GetController());
		if(PC)
		{
			PC->ClientPlayCameraShake(FireShake);
		}
	}
}


void AC_BaseGun::StartFire()
{
	// This ensures that a player cannot fire shots faster than fire rate by just rapidly clicking. 
	// Clamp this so if First delay is -ve use 0 -- Max uses largest value
	
	float FirstDelay = FMath::Max(LastFireTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);

	GetWorldTimerManager().SetTimer(AutomaticFireHandle, this, &AC_BaseGun::Fire, TimeBetweenShots, true, FirstDelay);
}

void AC_BaseGun::StopFire()
{
	GetWorldTimerManager().ClearTimer(AutomaticFireHandle);
}

void AC_BaseGun::Server_Fire_Implementation()
{
	//UE_LOG(LogTemp, Log, TEXT("CLIENT EXECUTED SERVER_FIRE"));
	Fire();
}

void AC_BaseGun::Multi_Fire_Implementation()
{
	Fire();
}


void AC_BaseGun::Server_StopFire_Implementation()
{
	Multi_StopFire();
}

void AC_BaseGun::Multi_StopFire_Implementation()
{
	StopFire();
}
