// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HaloReach/Interactables/Weapons/C_BaseWeapon.h"
#include "Components/TimelineComponent.h"
#include "C_BaseGun.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFireWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStopFireWeapon);


USTRUCT(BlueprintType)
struct FWeaponStats
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	float BaseDamage;

	// RPM
	UPROPERTY(EditDefaultsOnly)
	float RateOfFire;

	//UPROPERTY(EditDefaultsOnly)
	//float ReloadSpeed;

	// Max ammo in a magazine
	UPROPERTY(EditDefaultsOnly)
	int32 MaxMagazineAmmo;

	// Max ammo in reserves
	UPROPERTY(EditDefaultsOnly)
	int32 MaxReservesAmmo;

	UPROPERTY(EditDefaultsOnly)
	bool bCanZoom;

	// If camera pitch becomes greater than this a new original rotation will be set
	UPROPERTY(EditDefaultsOnly)
	float MaxPitchValue;

	// If camera pitch becomes less than this a new original rotation will be set
	UPROPERTY(EditDefaultsOnly)
	float MinPitchValue;

	// If camera yaw becomes greater than this a new original rotation will be set
	UPROPERTY(EditDefaultsOnly)
	float MaxYawValue;

	// If camera yaw becomes less than this a new original rotation will be set
	UPROPERTY(EditDefaultsOnly)
	float MinYawValue;

	// The Amount of bullet spread that occurs in the yaw direction
	UPROPERTY(EditDefaultsOnly)
	float BulletSpreadYaw;

	// The Amount of bullet spread that occurs in the pitch direction
	UPROPERTY(EditDefaultsOnly)
	float BulletSpreadPitch;


	// The Amount of bullet spread that occurs in the roll direction
	UPROPERTY(EditDefaultsOnly)
	float BulletSpreadRoll;


	// The speed of the recoil pattern timeline
	//UPROPERTY(EditDefaultsOnly)
	//float RecoiltimelinePlayRate;

	// The speed of the return timeline
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "1.0", ClampMax = "2.0"))
	float ReturntimelinePlayRate;

	float LastFireTime;

	float TimeBetweenShots;

	int32 CurrentAmmo;

	int32 CurrentReservesAmmo;

	// Time of reload animation 
	float ReloadLength;

	// Time of weapon switch animation 
	float SwitchLength;

	FWeaponStats()
	{
		BaseDamage = 10.0f;
		RateOfFire = 600.0f;

		//ReloadSpeed = 1.0f;
		MaxMagazineAmmo = 32;
		MaxReservesAmmo = 224;

		CurrentAmmo = MaxMagazineAmmo;
		CurrentReservesAmmo = MaxReservesAmmo;

		// These values act as the range for the recoil pattern. If the camera rotation leaves this range a new return position is set (for ReturnRecoil func)
		MaxPitchValue = 20.0f;
		MinPitchValue = 20.0f;
		MaxYawValue = 10.0f;
		MinYawValue = 10.0f;

		ReturntimelinePlayRate = 1.0f;

		BulletSpreadPitch = 0.1;
		BulletSpreadRoll = 0.1f;
		BulletSpreadYaw = 0.1f;

	}
};

UCLASS()
class HALOREACH_API AC_BaseGun : public AC_BaseWeapon
{
	GENERATED_BODY()

protected:

	AC_BaseGun();

	virtual void BeginPlay() override;

	virtual void Tick(float Delta) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gun | Weapon Effects")
	UParticleSystem* MuzzleEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gun | Weapon Effects")
	UParticleSystem* DefaultImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gun | Weapon Effects")
	UParticleSystem* FleshImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gun | Weapon Effects")
	FName MuzzleSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gun | Weapon Effects")
	TSubclassOf<UMatineeCameraShake> FireShake;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gun | Weapon Effects")
	TSubclassOf<UDamageType> DamageType;

	// A basic fire function which uses a line trace and applies damage, Will always be called on server
	void Fire();

	void FireHandling();

	// A Fire() function that is only called locally
	void LocalFire();

	// Plays particle effects for weapon
	void PlayFireEffects();

	FTimerHandle AutomaticLocalFireHandle;

	FTimerHandle SemiLocalFireHandle;


// AMMO COUNTER

	UPROPERTY(EditDefaultsOnly, Category = "Gun | Ammo Counter Images")
	TArray<UTexture2D*> AmmoCounterImageArray;

	void SetCurrentAmmoImage(UTexture2D* NewTexture);

	// Material Ammo Counter used on weapon mesh

	// Base material
	UPROPERTY(EditDefaultsOnly, Category = "Gun | Ammo Counter")
	UMaterialInterface* Material;

	// The name of the texture parameter
	UPROPERTY(EditDefaultsOnly, Category = "Gun | Ammo Counter")
	FName ParameterName;

	// The array of all the ammo counter images that will be used on the weapon mesh's display
	UPROPERTY(EditDefaultsOnly, Category = "Gun | Ammo Counter")
	TArray<UTexture2D* > MeshAmmoCounterArray;

	// The dynamic material to be used at runtime
	UMaterialInstanceDynamic* DynMaterial;

	// The index where the display ammo counter material is used, Set to -1 if no display counter is used
	UPROPERTY(EditDefaultsOnly, Category = "Gun | Ammo Counter")
	int DisplayMaterialIndex;

	void SetMeshAmmoCounter(UTexture2D* Texture);

// AUTOMATIC

	void StartAutoFire();
	void StopAutoFire();

	FTimerHandle AutomaticFireHandle;

	// Decides whether to use semi recoil or auto 
	bool bAutoWeapon;

// SEMI

	void StartSemiFire();

	FTimerHandle SemiFireHandle;

// RELOAD

	FTimerHandle ReloadHandle;

public:
	//UPROPERTY(Replicated)
	bool bCanFire;

	UFUNCTION(BlueprintCallable)
	void SetCanFire(bool x);

	UFUNCTION(Server, Reliable)
	void Server_SetCanFire(bool x);
	void Server_SetCanFire_Implementation(bool x);

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void ResetCanFire();

// WEAPON RECOIL 

	void StopRecoil();

	// Will set Original Rotation is the player's camera rotation moves outside a given range
	void ChangeOriginalRotation();

	// The rotation of the camera before recoil starts
	FRotator OriginalRotation;

	// Used as a do once bool, to set the original rotation
	bool bSetOriginalRotation;

	// Used to reset the recoil timeline, to decied if the timeline should continue or play from start
	bool bIsRecoilTimelineFinished;

	// RECOIL TIMELINE

	UTimelineComponent* RecoilTimeline;

	// The curve that drives the pitch of the recoil pattern
	UPROPERTY(EditDefaultsOnly, Category = "Gun | Weapon Recoil")
	UCurveFloat* FRecoilPitchCurve;

	// The curve that drives the yaw of the recoil pattern
	UPROPERTY(EditDefaultsOnly, Category = "Gun | Weapon Recoil")
	UCurveFloat* FRecoilYawCurve;

	// The curve that is used to set the timelines length, not used in recoil pattern
	UPROPERTY(EditDefaultsOnly, Category = "Gun | Weapon Recoil")
	UCurveFloat* FDefaultCurve;

	FOnTimelineFloat RecoilInterpFunction{};
	FOnTimelineEvent RecoilTimelineFinished{};

	UFUNCTION()
	void RecoilTimelineFloatReturn(float Alpha);

	UFUNCTION()
	void OnRecoilTimelineFinished();

	void StopRecoilTimeline();


	// TIMELINE UPDATE 

	// Used instead of AddInterpFloat func as that function does not support multiple curves
	FOnTimelineEvent RecoilTimelineUpdate;

	// A tick function while the timeline plays
	UFUNCTION()
	void OnTimelineUpdate();

	// Adds pitch and yaw input to the camera rotation moving the camera
	void UpdateRecoil(float Pitch, float Yaw);


	// WEAPON RETURN TIMELINE -- This is the timeline used to reset the rotation, make the weapon return to start position after firing

	UTimelineComponent* ReturnTimeline;

	// The curve used to return the weapon to its original rotation before recoil
	UPROPERTY(EditDefaultsOnly, Category = "Gun | Weapon Recoil")
	UCurveFloat* FReturnCurve;

	FOnTimelineFloat ReturnInterpFunction{};
	FOnTimelineEvent ReturnTimelineFinished{};

	UFUNCTION()
	void ReturnTimelineFloatReturn(float Value);

	UFUNCTION()
	void OnReturnTimelineFinished();

	// This will retunr the players camera rotation back to the original rotation
	void ReturnRecoil();

// WEAPON BULLET SPREAD

	FVector SpreadTrace(FVector Trace);

// Replication

	UFUNCTION(Server, Reliable)
	void Server_Fire();
	void Server_Fire_Implementation();

	UFUNCTION(NetMultiCast, Reliable)
	void Multi_Fire();
	void Multi_Fire_Implementation();

	UFUNCTION(Server, Reliable)
	void Server_StopFire();
	void Server_StopFire_Implementation();

	UFUNCTION(NetMultiCast, Reliable)
	void Multi_StopFire();
	void Multi_StopFire_Implementation();

private:

public:

	UFUNCTION(BlueprintCallable)
	void StartRecoil();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun | Weapon Stats")
	FWeaponStats WeaponStats;

	UTexture2D* CurrentAmmoImage;

	void Reload();

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnFireWeapon OnFireWeapon;

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnStopFireWeapon OnStopFireWeapon;

	void StopReturnTimeline();

	//Used to set the Ammo counter used in the HUD as well as the ammo counter on the weapon mesh if weapon has one
	UFUNCTION()
	virtual void UpdateAmmoCounter();
};
