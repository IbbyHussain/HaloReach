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

	//UPROPERTY(EditDefaultsOnly, Category = "Combat")
	//TSubclassOf<AC_BaseWeapon> DefaultWeaponClass;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<AC_Weapon3P> WeaponClass3P;

	// The weapon shown in thrid person.
	UPROPERTY()
	AC_Weapon3P* Weapon3P;


	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<AC_Weapon3P> HolsteredWeaponClass3P;

	// The Holstered weapon shown in thrid person.
	AC_Weapon3P* HolsteredWeapon3P;

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
	AC_BaseWeapon* CurrentWeapon;

	UPROPERTY(Replicated)
	AC_BaseWeapon* HolsteredWeapon;

	void SpawnWeapon(TSubclassOf<AC_BaseWeapon> WeaponClass, FName WeaponSocket);

	void SpawnWeapon3P(AC_Weapon3P*& Weapon, TSubclassOf<AC_Weapon3P> WeaponClass, FName SocketName);

	void UpdateWeapon3P(AC_Weapon3P*& Weapon, AC_BaseWeapon* NewWeapon, FName Weapon3PSocket);

	UFUNCTION()
	void SwitchWeapons();

	UFUNCTION(Server, Reliable)
	void Server_SwitchWeapons();
	void Server_SwitchWeapons_Implementation();

	UFUNCTION(NetMulticast, Reliable)
	void Multi_SwitchWeapons();
	void Multi_SwitchWeapons_Implementation();


	// Will handle and switch weapons depending on what weapons are equipped
	void UpdateWeapons();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player | PlayerComponents")
	EWeaponType WeaponType;

	// Applies any updates to the character when they change weapon type
	void OnWeaponTypeUpdate();

	void SetupWeaponSwitching(TSubclassOf<AC_BaseWeapon> WeaponClass, FName WeaponSocket, FName Weapon3PSocket, bool bUpdateCurrentWeapon);

	//void BasicSetupWeapon(TSubclassOf<AC_BaseWeapon> WeaponClass, FName WeaponSocket, FName Weapon3PSocket);

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

// REPLICATION TESTINGS

	void Update3PWeapons(FName Weapon3PSocket);


	UFUNCTION(Server, Reliable)
		void Server_Update3PWeapons(FName Weapon3PSocket);
	void Server_Update3PWeapons_Implementation(FName Weapon3PSocket);

	UFUNCTION(Client, Reliable)
	void Multi_Update3PWeapons(FName Weapon3PSocket);
	void Multi_Update3PWeapons_Implementation(FName Weapon3PSocket);










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

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<AC_BaseWeapon> DefaultWeaponClass;


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
