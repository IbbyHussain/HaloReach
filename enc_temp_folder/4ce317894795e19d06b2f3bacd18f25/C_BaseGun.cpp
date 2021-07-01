
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
	bReplicates = true;

	NetUpdateFrequency = 30.0f;
	MinNetUpdateFrequency = 10.0f;
}

void AC_BaseGun::BeginPlay()
{
	Super::BeginPlay();

	// Divide by 60 to get per minute
	WeaponStats.TimeBetweenShots = 60.0f / WeaponStats.RateOfFire;
}

void AC_BaseGun::Fire()
{
	AActor* MyOwner = GetOwner();

	if(!(MyOwner->HasAuthority()))
	{
		Server_Fire();
	}

	if(MyOwner)
	{
		UpdateAmmoCounter();

		if(WeaponStats.CurrentAmmo > 0)
		{
			//UE_LOG(LogTemp, Log, TEXT("SERVER FIRED"));

			WeaponStats.CurrentAmmo -= 1;

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

			if (bHit)
			{
				AActor* HitActor = Hit.GetActor();

				EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

				// Head shot damage should only be applied when shields = 0

				// Head shot multiplier 
				float ActualDamage = WeaponStats.BaseDamage;
				if (SurfaceType == SURFACE_FLESHVULNERABLE)
				{
					ActualDamage *= 3.0f;
				}

				UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, Hit, MyOwner->GetInstigatorController(), this, DamageType);

				DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::Red, false, 1.0f, 0, 1.0f);

				// Plays particle effect (Impact Effect) depending on physical material hit
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

			WeaponStats.LastFireTime = GetWorld()->TimeSeconds;
		}

		else 
		{
			// Auto reload if no ammo left
			Reload();
		}
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

void AC_BaseGun::Reload()
{
	WeaponStats.CurrentAmmo = WeaponStats.MaxMagazineAmmo;

	UpdateAmmoCounter();
}

void AC_BaseGun::StartAutoFire()
{
	// This ensures that a player cannot fire shots faster than fire rate by just rapidly clicking. 
	// Clamp this so if First delay is -ve use 0 -- Max uses largest value
	
	float FirstDelay = FMath::Max(WeaponStats.LastFireTime + WeaponStats.TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);
	GetWorldTimerManager().SetTimer(AutomaticFireHandle, this, &AC_BaseGun::Fire, WeaponStats.TimeBetweenShots, true, FirstDelay);

	UpdateAmmoCounter();
}

void AC_BaseGun::StopAutoFire()
{
	UpdateAmmoCounter();
	GetWorldTimerManager().ClearTimer(AutomaticFireHandle);
}

void AC_BaseGun::StartSemiFire()
{
	float FirstDelay = FMath::Max(WeaponStats.LastFireTime + WeaponStats.TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);
	GetWorldTimerManager().SetTimer(SemiFireHandle, this, &AC_BaseGun::Fire, WeaponStats.TimeBetweenShots, false, FirstDelay);

	UpdateAmmoCounter();
}

void AC_BaseGun::SetCurrentAmmoImage(UTexture2D* NewTexture)
{
	CurrentAmmoImage = NewTexture;
}

// Virtual Functions
void AC_BaseGun::UpdateAmmoCounter()
{

}









// Replication

void AC_BaseGun::Server_Fire_Implementation()
{
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
	StopAutoFire();
}
