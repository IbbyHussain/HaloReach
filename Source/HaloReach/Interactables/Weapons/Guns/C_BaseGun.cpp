
#include "HaloReach/Interactables/Weapons/Guns/C_BaseGun.h"
#include "DrawDebugHelpers.h"
#include "HaloReach/Player/C_PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Camera/CameraShake.h"
#include "HaloReach/HaloReach.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "TimerManager.h"
#include "Camera/CameraComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "UnrealClient.h"
#include "Kismet/KismetMathLibrary.h"

AC_BaseGun::AC_BaseGun()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	NetUpdateFrequency = 60.0f;
	MinNetUpdateFrequency = 10.0f;

	bCanFire = true;

	// Recoil Timeline

	RecoilTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("Recoil Timeline"));
	RecoilInterpFunction.BindUFunction(this, FName("RecoilTimelineFloatReturn"));
	RecoilTimelineFinished.BindUFunction(this, FName("OnRecoilTimelineFinished"));
	RecoilTimelineUpdate.BindUFunction(this, FName("OnTimelineUpdate"));

	ReturnTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("Return Timeline"));
	ReturnInterpFunction.BindUFunction(this, FName("ReturnTimelineFloatReturn"));
	ReturnTimelineFinished.BindUFunction(this, FName("OnReturnTimelineFinished"));

	ReturnTimeline->SetPlayRate(WeaponStats.ReturntimelinePlayRate);

	ParameterName = FName("CurrentAmmoImg");
	DisplayMaterialIndex = -1;
}

void AC_BaseGun::Tick(float Delta)
{
	Super::Tick(Delta);

	//UE_LOG(LogTemp, Error, TEXT("Status: %s "), (RecoilTimeline->IsPlaying() ? TEXT("TIMER IS PLAYING") : TEXT("TIMER IS NOT PLAYING")));
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("changed start rotation value"), (bHasTimerPaused ? TEXT("TIMER IS PAUSED") : TEXT("TIMER IS NOT PAUSED")));

}

void AC_BaseGun::BeginPlay()
{
	Super::BeginPlay();

	// Divide by 60 to get per minute
	WeaponStats.TimeBetweenShots = 60.0f / WeaponStats.RateOfFire;

	WeaponStats.ReloadLength = ReloadAnimation->GetPlayLength() / ReloadAnimation->RateScale;

	WeaponStats.SwitchLength = WeaponEquipAnimation->GetPlayLength() / WeaponEquipAnimation->RateScale;

	UpdateAmmoCounter();

	// Recoil Timeline

	if(FDefaultCurve)
	{
		RecoilTimeline->AddInterpFloat(FDefaultCurve, RecoilInterpFunction, FName("Alpha"));
		RecoilTimeline->SetTimelineFinishedFunc(RecoilTimelineFinished);
		RecoilTimeline->SetTimelinePostUpdateFunc(RecoilTimelineUpdate);
		RecoilTimeline->SetLooping(false);
	}

	if(FReturnCurve)
	{
		ReturnTimeline->AddInterpFloat(FReturnCurve, ReturnInterpFunction, FName("Charlie"));
		ReturnTimeline->SetTimelineFinishedFunc(ReturnTimelineFinished);
		ReturnTimeline->SetLooping(false);
	}

	// BP set values for MaxReservesAmmo is not set without this
	WeaponStats.CurrentReservesAmmo = WeaponStats.MaxReservesAmmo;

	// Create dynamic material

	if(DisplayMaterialIndex != -1)
	{
		DynMaterial = UMaterialInstanceDynamic::Create(Material, this);
		WeaponMesh->SetMaterial(DisplayMaterialIndex, DynMaterial);
	}
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
		if(WeaponStats.CurrentAmmo > 0 && bCanFire)
		{
			WeaponStats.CurrentAmmo -= 1;
			UpdateAmmoCounter();

			AC_PlayerCharacter* PlayerCharacter = Cast<AC_PlayerCharacter>(MyOwner);
			if(PlayerCharacter)
			{
				PlayerCharacter->OnWeaponFire();
				PlayerCharacter->PlayMontage(GetWeaponFireMontage());
			}

			FVector EyeLocation;
			FRotator EyeRotation;
			MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

			FVector ShotDirection = EyeRotation.Vector();
			FVector TraceEnd = EyeLocation + (SpreadTrace(ShotDirection) * 5000);

			FCollisionQueryParams QueryParams;

			QueryParams.AddIgnoredActor(MyOwner);
			QueryParams.AddIgnoredActor(this);
			QueryParams.bTraceComplex = true;
			QueryParams.bReturnPhysicalMaterial = true;

			FHitResult Hit;

			bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryParams);
			DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::Red, false, 1.0f, 0, 1.0f);

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

		else if(WeaponStats.CurrentReservesAmmo != 0)
		{
			// Auto reload if no ammo left
			AC_PlayerCharacter* PlayerCharacter = Cast<AC_PlayerCharacter>(MyOwner);
			if(PlayerCharacter)
			{
				PlayerCharacter->Reload(); // Calls player reload, so montages can be played
				Reload();
				PlayerCharacter->OnWeaponStopFire(); // Stops 3p fire anim
				PlayerCharacter->StopMontage(GetWeaponFireMontage()); // stops 1p fire anim
			}
		}

		else
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), DryFireSound, GetActorLocation());
		}
	}
}

void AC_BaseGun::LocalFire()
{
	if (WeaponStats.CurrentAmmo > 0 && bCanFire)
	{
		StartRecoil();
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
	// If we dont have max ammo in magazine
	if (WeaponStats.CurrentAmmo != WeaponStats.MaxMagazineAmmo)
	{
		// push unused ammo back into reserves
		WeaponStats.CurrentReservesAmmo += WeaponStats.CurrentAmmo;

		// if there is enough ammo in reserves to fully restock current magazine, then do so
		if (WeaponStats.CurrentReservesAmmo >= WeaponStats.MaxMagazineAmmo)
		{
			WeaponStats.CurrentReservesAmmo -= WeaponStats.MaxMagazineAmmo;
			WeaponStats.CurrentAmmo = WeaponStats.MaxMagazineAmmo;
		}

		// if there is not enough ammo in reserves to give a full mag, set current mag to whatever ammo is left in reserves
		else if (WeaponStats.CurrentReservesAmmo > 0 && WeaponStats.CurrentReservesAmmo < WeaponStats.MaxMagazineAmmo)
		{
			WeaponStats.CurrentAmmo = WeaponStats.CurrentReservesAmmo;
			WeaponStats.CurrentReservesAmmo = 0;
		}

		StopRecoilTimeline();
		ReturnRecoil(); // temp tests

		bIsRecoilTimelineFinished = true;

		// Time before can fire again 
		bCanFire = false;
		GetWorldTimerManager().SetTimer(ReloadHandle, this, &AC_BaseGun::ResetCanFire, WeaponStats.ReloadLength, false);
	}
}

void AC_BaseGun::Server_Reload_Implementation()
{
	Reload();
}

void AC_BaseGun::ResetCanFire()
{
	bCanFire = true;
	GetWorldTimerManager().ClearTimer(ReloadHandle);
}

void AC_BaseGun::StartAutoFire()
{
	// This ensures that a player cannot fire shots faster than fire rate by just rapidly clicking. 
	// Clamp this so if First delay is -ve use 0 -- Max uses largest value
	
	float FirstDelay = FMath::Max(WeaponStats.LastFireTime + WeaponStats.TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);
	GetWorldTimerManager().SetTimer(AutomaticFireHandle, this, &AC_BaseGun::Fire, WeaponStats.TimeBetweenShots, true, FirstDelay);
	GetWorldTimerManager().SetTimer(AutomaticLocalFireHandle, this, &AC_BaseGun::LocalFire, WeaponStats.TimeBetweenShots, true, FirstDelay);
}

void AC_BaseGun::StopAutoFire()
{
	AActor* MyOwner = GetOwner();
	if(MyOwner)
	{
		AC_PlayerCharacter* PlayerCharacter = Cast<AC_PlayerCharacter>(MyOwner);
		if (PlayerCharacter)
		{
			PlayerCharacter->OnWeaponStopFire();
			PlayerCharacter->StopMontage(GetWeaponFireMontage());
			StopRecoil();
		}
	}
	
	GetWorldTimerManager().ClearTimer(AutomaticFireHandle);
	GetWorldTimerManager().ClearTimer(AutomaticLocalFireHandle);
}

void AC_BaseGun::StartSemiFire()
{
	float FirstDelay = FMath::Max(WeaponStats.LastFireTime + WeaponStats.TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);
	GetWorldTimerManager().SetTimer(SemiFireHandle, this, &AC_BaseGun::Fire, WeaponStats.TimeBetweenShots, false, FirstDelay);
	GetWorldTimerManager().SetTimer(SemiLocalFireHandle, this, &AC_BaseGun::LocalFire, WeaponStats.TimeBetweenShots, false, FirstDelay);
}

void AC_BaseGun::SetCurrentAmmoImage(UTexture2D* NewTexture)
{
	CurrentAmmoImage = NewTexture;
}

// WEAPON RECOIL 

//commented out
void AC_BaseGun::StartRecoil()
{
	//AActor* MyOwner = GetOwner();
	//if (MyOwner)
	//{
	//	AC_PlayerCharacter* PlayerCharacter = Cast<AC_PlayerCharacter>(MyOwner);
	//	if (PlayerCharacter)
	//	{
	//		StopReturnTimeline();

	//		// Should only be set once, when player first fires
	//		if(bSetOriginalRotation)
	//		{
	//			OriginalRotation = PlayerCharacter->CameraComp->GetComponentRotation(); // CameraComp
	//			bSetOriginalRotation = false;
	//		}

	//		if(bAutoWeapon)
	//		{
	//			// If the timeline is finished play from begining, so that a new recoil pattern starts
	//			if (bIsRecoilTimelineFinished)
	//			{
	//				bIsRecoilTimelineFinished = false;
	//				RecoilTimeline->PlayFromStart();
	//			}

	//			// Continue form when timeline stopped so that the recoil pattern completes
	//			else
	//			{
	//				RecoilTimeline->Play();
	//			}
	//		}

	//		// Semi weapons should start from beging as timeline finishes for each shot
	//		else
	//		{
	//			RecoilTimeline->PlayFromStart();
	//		}
	//	}
	//}
}

//commented out
void AC_BaseGun::StopRecoil()
{
	// Only reset for auto weapon
	/*if(bAutoWeapon)
	{
		StopRecoilTimeline();
		bSetOriginalRotation = true;
		ReturnRecoil();
	}*/
}

void AC_BaseGun::RecoilTimelineFloatReturn(float Alpha)
{

}

void AC_BaseGun::OnRecoilTimelineFinished()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if(PC)
	{
		// Have to set Roll to 0.0f as during timeloine it changes slighlty due to gimbal lock
		float P = PC->GetControlRotation().Pitch;
		float Y = PC->GetControlRotation().Yaw;
		PC->SetControlRotation(FRotator(P, Y, 0.0f));
	}
	ReturnRecoil();
}

void AC_BaseGun::StopRecoilTimeline()
{
	if (RecoilTimeline->IsPlaying())
	{
		RecoilTimeline->Stop();
	}
}

void AC_BaseGun::ChangeOriginalRotation()
{
	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		AC_PlayerCharacter* PlayerCharacter = Cast<AC_PlayerCharacter>(MyOwner);
		if (PlayerCharacter)
		{
			UCameraComponent* Cam = PlayerCharacter->CameraComp;

			// Pitch setup
			float CameraPitch = Cam->GetComponentRotation().Pitch;
			
			float MaxPitch = OriginalRotation.Pitch + WeaponStats.MaxPitchValue;
			float MinPitch = OriginalRotation.Pitch - WeaponStats.MinPitchValue;

			// If the players camera pitch goes out of range
			bool bPitch = CameraPitch >= MaxPitch || CameraPitch <= MinPitch;

			// Yaw setup
			float CameraYaw = Cam->GetComponentRotation().Yaw;

			float MaxYaw = OriginalRotation.Yaw + WeaponStats.MaxYawValue;
			float MinYaw = OriginalRotation.Yaw - WeaponStats.MinYawValue;

			//If the players camera Yaw goes out of range
			bool bYaw = CameraYaw >= MaxYaw || CameraYaw <= MinYaw;

			// If the palyers yaw or pitch goes out of range a new original rotation should be set. This stops the return timeline moving too much
			if (bPitch || bYaw)
			{
				OriginalRotation = Cam->GetComponentRotation();
			}
		}
	}
}

void AC_BaseGun::UpdateRecoil(float Pitch, float Yaw)
{
	ChangeOriginalRotation();

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (PC)
	{
		if(WeaponStats.CurrentAmmo > 0)
		{
			// Multiplied by delta seconds to get frame rate independence. Multiplied by another value as otherwise teh value is too small
			PC->AddPitchInput(Pitch * GetWorld()->GetDeltaSeconds() * 120.0f);
			PC->AddYawInput(Yaw * GetWorld()->GetDeltaSeconds() * 120.0f);
		}
		
		else if(WeaponStats.CurrentReservesAmmo == 0)
		{
			StopRecoil();
		}
	}
}

void AC_BaseGun::OnTimelineUpdate()
{
	// While the timeline updates (ticks), the recoil updates, using a different curve for pitch and yaw
	UpdateRecoil(FRecoilPitchCurve->GetFloatValue(RecoilTimeline->GetPlaybackPosition()), 
		FRecoilYawCurve->GetFloatValue(RecoilTimeline->GetPlaybackPosition()));
}

void AC_BaseGun::ReturnTimelineFloatReturn(float Value)
{
	float MouseX, MouseY;

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	PC->GetInputMouseDelta(MouseX, MouseY);

	// Checks for mouse input above a range
	bool bMouseX = MouseX >= 0.5 || MouseX <= -0.5;
	bool bMouseY = MouseY >= 0.5 || MouseY <= -0.5;

	// If there is mouse input stop this timeline
	if(MouseX || MouseY)
	{
		StopReturnTimeline();
	}

	else
	{
		float CPitch = PC->GetControlRotation().Pitch;
		float CYaw = PC->GetControlRotation().Yaw;
		float CRoll = PC->GetControlRotation().Roll;

		float OriginalPitch = OriginalRotation.Pitch;
		float OriginalYaw = OriginalRotation.Yaw;

		// Lerps from current rotation to the original rotation, but does not include roll
		FRotator NewRotation = UKismetMathLibrary::RLerp(FRotator(CPitch, CYaw, CRoll), FRotator(OriginalPitch, OriginalYaw, CRoll), 
			Value * WeaponStats.ReturntimelinePlayRate, true);

		PC->SetControlRotation(NewRotation);
	}

	if (PC)
	{
		// Have to set Roll to 0.0f as during timeloine it changes slighlty due to gimbal lock
		float P = PC->GetControlRotation().Pitch;
		float Y = PC->GetControlRotation().Yaw;
		PC->SetControlRotation(FRotator(P, Y, 0.0f));
	}
}

void AC_BaseGun::OnReturnTimelineFinished()
{
	StopReturnTimeline();
}

void AC_BaseGun::StopReturnTimeline()
{
	if (ReturnTimeline->IsPlaying())
	{
		ReturnTimeline->Stop();
	}
}

//commented out
void AC_BaseGun::ReturnRecoil()
{
	if(!ReturnTimeline->IsPlaying())
	{
		//ReturnTimeline->PlayFromStart();
	}
}

// WEAPON BULLET SPREAD

FVector AC_BaseGun::SpreadTrace(FVector Trace)
{
	float X = Trace.X;
	float Y = Trace.Y;
	float Z = Trace.Z;

	X += UKismetMathLibrary::RandomFloatInRange(WeaponStats.BulletSpreadRoll * -1.0f, WeaponStats.BulletSpreadRoll);
	Y += UKismetMathLibrary::RandomFloatInRange(WeaponStats.BulletSpreadPitch * -1.0f, WeaponStats.BulletSpreadPitch);
	Z += UKismetMathLibrary::RandomFloatInRange(WeaponStats.BulletSpreadYaw * -1.0f, WeaponStats.BulletSpreadYaw);

	return FVector(X, Y , Z);
}

// AMMO COUNTER

void AC_BaseGun::SetMeshAmmoCounter(UTexture2D* Texture)
{
	// Updates the texture
	if (DisplayMaterialIndex != -1 && DynMaterial)
	{
		DynMaterial->SetTextureParameterValue(ParameterName, Texture);
	}
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

// Virtual Functions

void AC_BaseGun::UpdateAmmoCounter()
{

}

void AC_BaseGun::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AC_BaseGun, WeaponStats);
	//DOREPLIFETIME(AC_BaseGun, WeaponStats.CurrentAmmo);

}
