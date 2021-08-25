// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Runtime/Engine/Classes/Components/TimelineComponent.h" 
#include "HaloReach/Interfaces/C_InteractInterface.h"
#include "HaloReach/GlobalEnums.h"
#include "Components/TimelineComponent.h"

#include "HaloReach/Player/PlayerExtra/C_PlayerCMC.h"

#include "C_PlayerCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponReload);

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

	AC_PlayerCharacter(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	FORCEINLINE class UC_PlayerCMC* GetPlayerMovementComponent() const { return PlayerCMC; }

	// Custom character movement component
	UC_PlayerCMC* PlayerCMC;

// CONTAINER DECLARATIONS

	FMovement Movement;

	EMovementState MovementState;

	UPROPERTY(Replicated)
	ECombatState CombatState;

// OVERRIDE METHODS

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void Landed(const FHitResult& Hit) override;

	virtual void PostInitializeComponents() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

// PLAYER COMPONENTS

	// The character mesh used when in first person
	UPROPERTY(EditDefaultsOnly, Category = "Player | PlayerComponents")
	USkeletalMeshComponent* DefaultMesh;

	// The character mesh used when in third person
	UPROPERTY(EditDefaultsOnly, Category = "Player | PlayerComponents")
	USkeletalMeshComponent* Mesh3P;

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

	void PlayMontage(USkeletalMeshComponent* MeshComp, UAnimMontage* Montage);

	void StopMontage(USkeletalMeshComponent* MeshComp, UAnimMontage* Montage);


// WEAPON AIM OFFSET

	// Used to replicate player rotation, used in ABP for aim offset
	UPROPERTY(BlueprintReadWrite, Replicated)
	FRotator ControlRotation;

	UFUNCTION(BlueprintCallable)
	void SetControlRotation();
	

// WEAPON FIRE

	UPROPERTY(Replicated)
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

# pragma region Weapon Switching

	bool bCanSwitch;

	FTimerHandle SwitchResetHandle;

	void ResetCanSwitch();

# pragma endregion

// WEAPON RELOADING

	UFUNCTION(Server, Reliable)
	void Server_PlayMontage(USkeletalMeshComponent* MeshComp, UAnimMontage* Montage);
	void Server_PlayMontage_Implementation(USkeletalMeshComponent* MeshComp, UAnimMontage* Montage);

	UFUNCTION(NetMultiCast, Reliable)
	void Multi_PlayMontage(USkeletalMeshComponent* MeshComp, UAnimMontage* Montage);
	void Multi_PlayMontage_Implementation(USkeletalMeshComponent* MeshComp, UAnimMontage* Montage);


	bool bCanReload;

	FTimerHandle ReloadResetHandle;

	void ResetCanReload();

	void Reload();


# pragma region Melee Attack

	// Plays montage for melee attack - input binding
	void StartMelee();

	void ResetMelee();

	FTimerHandle MeleeHandle;

	bool bCanMelee;

	// Method will use sphere traces to deal damage, will be used in an anim notify state
	void MeleeAttack(USkeletalMeshComponent* MeshComp);

	TArray<AActor*> ActorsIgnored;

	TArray<AC_PlayerCharacter*> HitPlayers;

	void ClearActorsIgnoredArray();

	UFUNCTION(Server, Reliable)
	void Server_MeleeAttack(AC_PlayerCharacter* HitActor);
	void Server_MeleeAttack_Implementation(AC_PlayerCharacter* HitActor);

	// Melee Tracking

	void MeleeTracking();

	TArray<AActor*> IgnoredActorsTracking;

	// Where the melee track BOX should start
	UPROPERTY(EditDefaultsOnly, Category = "Player | Melee Tracking")
	FName MeleeStartSocket;

	UPROPERTY(EditDefaultsOnly, Category = "Player | Melee Tracking")
	UCurveFloat* FMeleeTrackCurve;

	UTimelineComponent* MeleeTrackTimeline;

	FOnTimelineFloat MeleeTrackInterpFunction{};
	FOnTimelineEvent MeleeTrackTimelineFinished{};

	UFUNCTION()
		void MeleeTrackTimelineFloatReturn(float Value);

	UFUNCTION()
		void OnMeleeTrackTimelineFinished();

	bool bIsOverlappingEnemy(AC_PlayerCharacter* Enemy);

	FVector EnemyLocation;
	FVector PlayerLoc;

	UFUNCTION(Server, Reliable)
		void Server_StartMelee(FVector StartLocation, FVector EndLocation, float Alpha);
	void Server_StartMelee_Implementation(FVector StartLocation, FVector EndLocation, float Alpha);

public: // temp
	UFUNCTION(BlueprintCallable)
		void StopMeleeTrackTimeline();

	TMap<float, AC_PlayerCharacter*> EnemyMap;

	float LocX;
	float LocY;
	float LocZ;

	float ShortestDistance;

# pragma endregion
	
// REPLICATION TESTINGS

	// Arraty which keeps track of equipped weapons
	UPROPERTY(Transient, BlueprintReadOnly, Replicated)
	TArray<AC_BaseWeapon*>EquippedWeaponArray;

	void WeaponArrayChecks();

	void SwitchWeapons();

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

	UFUNCTION(Server, Reliable)
	void Server_Jump();
	void Server_Jump_Implementation();


// CROUCH

	UPROPERTY(EditDefaultsOnly, Category = "Player | Speeds")
	float DefaultSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "Player | Speeds")
	float CrouchSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "Player | Sounds")
	USoundBase* CrouchStartSound;

	UPROPERTY(EditDefaultsOnly, Category = "Player | Sounds")
	USoundBase* CrouchEndSound;


	void StartCrouch();

	UFUNCTION(Server, Reliable)
	void Server_CrouchTimeline(bool bCrouch);
	void Server_CrouchTimeline_Implementation(bool bCrouch);

	UFUNCTION(Server, Reliable)
	void Server_CrouchSpeed();
	void Server_CrouchSpeed_Implementation();

	void SetCrouchKeyDown(bool bCrouch);

	UFUNCTION(Server, Reliable)
	void Server_Crouch(float Height, float NewHeight, float MeshHeight, float NewMeshHeight, float Alpha);
	void Server_Crouch_Implementation(float Height, float NewHeight, float MeshHeight, float NewMeshHeight, float Alpha);

	UFUNCTION(NetMultiCast, Reliable)
	void Multi_Crouch(float Height, float NewHeight, float MeshHeight, float NewMeshHeight, float Alpha);
	void Multi_Crouch_Implementation(float Height, float NewHeight, float MeshHeight, float NewMeshHeight, float Alpha);

	void EndCrouch();

	class UTimelineComponent* CrouchTimeline;

	UPROPERTY(EditAnywhere, Category = "Timeine")
	UCurveFloat* FCrouchCurve;

	UFUNCTION()
	void CrouchTimelineFloatReturn(float Value);

	FOnTimelineFloat CrouchInterpFunction{};

	UPROPERTY(Replicated)
	float DefaultCameraHeight;

	UPROPERTY(Replicated)
	float DefaultCapsuleHeight;

	UPROPERTY(Replicated)
	float DefaultMeshHeight;

	UPROPERTY(Replicated)
	float CrouchedCameraHeight;

	UPROPERTY(Replicated)
	float CrouchedCapsuleHeight;

	UPROPERTY(Replicated)
	float CrouchedMeshHeight;

	UPROPERTY(EditDefaultsOnly, Category = "Player | Crouch")
	float CrouchedCameraHeightMultiplier;

	UPROPERTY(EditDefaultsOnly, Category = "Player | Crouch")
	float CrouchedCapsuleHeightMultiplier;

	UPROPERTY(EditDefaultsOnly, Category = "Player | Crouch")
	float CrouchedMeshHeightMultiplier;

	bool bCanCrouch;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadWrite, Category = "Player | Crouch")
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

	UPROPERTY(EditDefaultsOnly, Category = "Player | PlayerComponents")
	class UCameraComponent* CameraComp;

	UPROPERTY(Replicated, EditDefaultsOnly, Category = "Player | Combat")
	FCombat Combat;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player | PlayerComponents")
	class UC_HealthComponent* HealthComp;


	void UpdateReserveAmmo();

	UPROPERTY(BlueprintAssignable, Category = "aaa")
	FOnWeaponReload OnWeaponReload;

	// Calls the reload function from the gun class
	void GunReload(AC_BaseGun* Gun);

	// Need a server RPC as all gun code is server side
	UFUNCTION(Server, Reliable)
	void Server_GunReload(AC_BaseGun* Gun);
	void Server_GunReload_Implementation(AC_BaseGun* Gun);

	// Weapon Mag

	AActor* Weapon1PMag;

	AActor* Weapon3PMag;

	USkeletalMeshComponent* GetDefaultMesh() const
	{
		return DefaultMesh;
	}

	USkeletalMeshComponent* GetMesh3P() const
	{
		return Mesh3P;
	}
};
