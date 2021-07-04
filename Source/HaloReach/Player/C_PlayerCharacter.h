// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Runtime/Engine/Classes/Components/TimelineComponent.h" 
#include "HaloReach/Interfaces/C_InteractInterface.h"
#include "HaloReach/GlobalEnums.h"
#include "C_PlayerCharacter.generated.h"

class AC_BaseWeapon;
class AC_Weapon3P;

UENUM()
enum class EMovementState : uint8
{
	IDLE,
	WALK,
	SPRINT,
	CROUCH,
	JUMP
};

UENUM()
enum class ECombatState : uint8
{
	DEFAULT,
	COMBAT
};

USTRUCT()
struct FMovement
{
	GENERATED_BODY() 
public:
	
	// JUMP

	FVector JumpHeight;

	FVector PlayersVelocity;
	float PlayersVelocityMagnitude;

	FMovement()
	{
		JumpHeight = FVector(0.0f, 0.0f, 500.0f);
	}
};

USTRUCT()
struct FCombat
{
	GENERATED_BODY()
public:

	// Shield Regen

	/*UPROPERTY(Replicated)
	AC_BaseWeapon* CurrentWeapon;

	UPROPERTY(Replicated)
	AC_BaseWeapon* HolsteredWeapon;*/

	UPROPERTY(EditDefaultsOnly, Category = "Combat | StarterWeapons")
	TSubclassOf<AC_BaseWeapon> DefaultPrimaryWeaponClass;

	UPROPERTY(EditDefaultsOnly, Category = "Combat | StarterWeapons")
	TSubclassOf<AC_BaseWeapon> DefaultSecondaryWeaponClass;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<AC_Weapon3P> PrimaryWeapon3PClass;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<AC_Weapon3P> SecondaryWeapon3PClass;


	FCombat()
	{
		//CurrentWeapon = nullptr;
		//HolsteredWeapon = nullptr;
	}

};

UCLASS()
class HALOREACH_API AC_PlayerCharacter : public ACharacter
{
	GENERATED_BODY()

private:

	AC_PlayerCharacter();

// CONTAINER DECLARATIONS

	FMovement Movement;

	EMovementState MovementState;

	UPROPERTY(Replicated)
	ECombatState CombatState;

// OVERRIDE METHODS

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void Landed(const FHitResult& Hit) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

// PLAYER COMPONENTS

	// The character mesh used when in first person
	UPROPERTY(EditDefaultsOnly, Category = "Player | PlayerComponents")
	USkeletalMeshComponent* DefaultMesh;

	// The character mesh used when in third person
	UPROPERTY(EditDefaultsOnly, Category = "Player | PlayerComponents")
	USkeletalMeshComponent* Mesh3P;

	UPROPERTY(EditDefaultsOnly, Category = "Player | PlayerComponents")
	class UCameraComponent* CameraComp;

// MOVEMENT METHODS

	void UpdateMovementSettings(EMovementState NewState);

	UFUNCTION()
	void UpdateCombatState(ECombatState NewState);

	
// INTERACT SYSTEM

	void Interact();

	UFUNCTION(Server, Reliable)
	void Server_Interact(FHitResult Hit);
	void Server_Interact_Implementation(FHitResult Hit);

	UFUNCTION(NetMulticast, Reliable)
	void Multi_Interact(FHitResult Hit);
	void Multi_Interact_Implementation(FHitResult Hit);


public:

// COMBAT SYSTEM

	UPROPERTY(Replicated)
	AC_BaseWeapon* PrimaryWeapon;

	UPROPERTY(Replicated)
	AC_BaseWeapon* SecondaryWeapon;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player | PlayerComponents")
	//EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Defaults, Replicated)
	TEnumAsByte<EWeaponType> WeaponType;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Applies any updates to the character when they change weapon type
	void OnWeaponTypeUpdate();

	
	

// WEAPON FIRE

	bool bCanFire;

	UPROPERTY(ReplicatedUsing = OnRep_Fire)
	bool bIsFiring;

	UFUNCTION()
	void OnRep_Fire();

	UFUNCTION(Server, Reliable)
	void Server_Fire(UAnimMontage* Montage);
	void Server_Fire_Implementation(UAnimMontage* Montage);

	UPROPERTY(ReplicatedUsing = OnRep_StopFire)
	bool bStopFiring;

	UFUNCTION()
	void OnRep_StopFire();

	UFUNCTION(Server, Reliable)
	void Server_StopFire(UAnimMontage* Montage);
	void Server_StopFire_Implementation(UAnimMontage* Montage);

	UFUNCTION()
	void OnWeaponFire();

	UFUNCTION()
	void OnWeaponStopFire();

// WEAPON SWITCHING

	bool bCanSwitch;

	FTimerHandle SwitchResetHandle;

	void ResetCanSwitch();

// WEAPON RELOADING

	UPROPERTY(ReplicatedUsing = OnRep_Reload)
	bool bIsReloading;

	UFUNCTION()
	void OnRep_Reload();

	UFUNCTION(Server, Reliable)
	void Server_Reload(UAnimMontage* Montage);
	void Server_Reload_Implementation(UAnimMontage* Montage);

	bool bCanReload;

	FTimerHandle ReloadResetHandle;

	void ResetCanReload();

	void Reload();

// REPLICATION TESTINGS

	// Will spawn a weapon of any type, in 1P
	void SpawnWeapon(TSubclassOf<AC_BaseWeapon> WeaponClass, AC_BaseWeapon*& Weapon, FName WeaponSocket);

	// Arraty which keeps track of equipped weapons
	UPROPERTY(Transient, BlueprintReadOnly, Replicated)
	TArray<AC_BaseWeapon*>EquippedWeaponArray;

	void WeaponArrayChecks();

	void SwitchWeapons();

	void SpawnWeapon3P(TSubclassOf<AC_Weapon3P> WeaponClass, AC_Weapon3P*& Weapon, FName WeaponSocket);

	// Arraty which keeps track of equipped weapons in 3P
	UPROPERTY(Transient, Replicated)
	TArray<AC_Weapon3P*> EquippedWeapon3PArray;

	void WeaponArray3PChecks();

	// The weapon shown in thrid person.
	UPROPERTY(Replicated)
	AC_Weapon3P* Primary3PWeapon;

	// The Holstered weapon shown in thrid person.
	UPROPERTY(Replicated)
	AC_Weapon3P* Secondary3PWeapon;

	bool bSwitch;

	UFUNCTION(Server, Reliable)
	void Server_WeaponArray3PChecks(EWeaponType NewType, FName Socket3P, FName Socket3PHolstered);
	void Server_WeaponArray3PChecks_Implementation(EWeaponType NewType, FName Socket3P, FName Socket3PHolstered);


// Replicated change skeletal mesh

	UFUNCTION(Server, Reliable)
	void Server_ChangeSkeletalMesh(USkeletalMesh* SKMesh, USkinnedMeshComponent* SkinnedMesh);
	void Server_ChangeSkeletalMesh_Implementation(USkeletalMesh* SKMesh, USkinnedMeshComponent* SkinnedMesh);

	UFUNCTION(NetMulticast, Reliable)
	void Multi_ChangeSkeletalMesh(USkeletalMesh* SKMesh, USkinnedMeshComponent* SkinnedMesh);
	void Multi_ChangeSkeletalMesh_Implementation(USkeletalMesh* SKMesh, USkinnedMeshComponent* SkinnedMesh);

	UPROPERTY(EditDefaultsOnly, Category = "Player | PlayerComponents")
	USkeletalMesh* TestMesh;












	// Weapons -----

	virtual FVector GetPawnViewLocation() const override;

	void StartFire();

	void EndFire();

	bool bZoomIn;

	UPROPERTY(EditDefaultsOnly, Category = "Player | PlayerComponents")
	float ZoomedFOV;

	UPROPERTY(EditDefaultsOnly, Category = "Player | PlayerComponents")
	float ZoomInterpSpeed;

	float DefaultFOV;

	void BeginZoom();

	void EndZoom();

	bool bCanZoom;

// Class Declarations

	class AC_PlayerHUD* HUD;

	// Movement

	FName WeaponAttachSocketName;

	void MoveForward(float Axis);

	void MoveRight(float Axis);

	bool CheckIdle();

	// JUMP

	bool bCanJump;

	void JumpStart();

	FTimerHandle JumpCooldownHandle;

	void ResetJump();

	// CROUCH

	void StartCrouch();

	void EndCrouch();

	class UTimelineComponent* CrouchTimeline;

	UPROPERTY(EditAnywhere, Category = "Timeine")
	UCurveFloat* FCrouchCurve;

	UFUNCTION()
	void CrouchTimelineFloatReturn(float Value);

	FOnTimelineFloat CrouchInterpFunction{};

	float DefaultCameraHeight;
	float DefaultCapsuleHeight;
	float DefaultMeshHeight;

	float CrouchedCameraHeight;
	float CrouchedCapsuleHeight;
	float CrouchedMeshHeight;

	UPROPERTY(EditDefaultsOnly, Category = "Player | Crouch")
	float CrouchedCameraHeightMultiplier;

	UPROPERTY(EditDefaultsOnly, Category = "Player | Crouch")
	float CrouchedCapsuleHeightMultiplier;

	UPROPERTY(EditDefaultsOnly, Category = "Player | Crouch")
	float CrouchedMeshHeightMultiplier;

	bool bCanCrouch;
	bool bCrouchKeyDown;

	float MeshX;
	float MeshY;

	class AC_PlayerCameraManager* CameraManager;

	UFUNCTION()
	void OnHealthChanged(UC_HealthComponent* HealthComponent, float Health, bool bUpdateCombatState);

	UFUNCTION()
	void OnSheildsChanged(UC_HealthComponent* HealthComponent, float Shields);

	FTimerHandle OutOfCombatHandle;

	FTimerDelegate OutOfCombatDelegate;

protected:

public:

	UPROPERTY(Replicated, EditDefaultsOnly, Category = "Player | Combat")
	FCombat Combat;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player | PlayerComponents")
	class UC_HealthComponent* HealthComp;

};
