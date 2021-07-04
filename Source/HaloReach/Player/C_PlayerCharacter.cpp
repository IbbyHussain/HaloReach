
#include "C_PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerExtra/C_PlayerCameraManager.h"
#include "HaloReach/Components/C_HealthComponent.h"
#include "HaloReach/UI/HUD/C_PlayerHUD.h"
#include "DrawDebugHelpers.h"
#include "HaloReach/Interactables/C_BasePickup.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/CollisionProfile.h"
#include <HaloReach/HaloReach.h>
#include "HaloReach/Interactables/Weapons/C_BaseWeapon.h"
#include "HaloReach/Interactables/Weapons/C_Weapon3P.h"
#include "HaloReach/Interactables/Weapons/Guns/C_BaseGun.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetArrayLibrary.h"

AC_PlayerCharacter::AC_PlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	WeaponAttachSocketName = ("R_GunSocket");

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComp->SetupAttachment(GetCapsuleComponent());
	//CameraComp->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	CameraComp->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	DefaultMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Meshcomp"));
	// Can only be seen by the local player, other clients cannot see 1P mesh
	DefaultMesh->SetOnlyOwnerSee(true);
	DefaultMesh->SetupAttachment(CameraComp);
	DefaultMesh->bCastDynamicShadow = false;
	DefaultMesh->CastShadow = false;
	//DefaultMesh->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	//DefaultMesh->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	// Create a mesh component that will be used when being viewed from a '3rd person' view (when controlling this pawn)
	Mesh3P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("3P Mesh"));
	// The local player will not see the Mesh3P SKMesh only other clients
	Mesh3P->bOwnerNoSee = true;
	Mesh3P->SetupAttachment(GetCapsuleComponent());
	// Use this mesh's shadows
	Mesh3P->bCastDynamicShadow = true;
	Mesh3P->CastShadow = true;

	HealthComp = CreateDefaultSubobject<UC_HealthComponent>(TEXT("HealthComponent"));

	GetCharacterMovement()->MaxWalkSpeed = 500.0f;

	//Crouch Timeline
	CrouchTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("CrouchTimeline"));
	CrouchInterpFunction.BindUFunction(this, FName("CrouchTimelineFloatReturn"));

	bCanCrouch = true;
	bCrouchKeyDown = false;

	MeshX = 0.0f;
	MeshY = 0.0f;

	MovementState = EMovementState::WALK;
	CombatState = ECombatState::DEFAULT;

	CrouchedCapsuleHeightMultiplier = 0.7f;
	CrouchedCameraHeightMultiplier = 0.5f;
	CrouchedMeshHeightMultiplier = 1.5f;

	ZoomedFOV = 65.0f;

	ZoomInterpSpeed = 20.0f;

	bCanSwitch = true;
	bCanZoom = true;
	bCanReload = true;
	bCanFire = true;

	bIsReloading = false;
	bIsFiring = false;


}

void AC_PlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	CameraManager = Cast<AC_PlayerCameraManager>(UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0));

	HUD = Cast<AC_PlayerHUD>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD());
	//HUD = Cast<AC_PlayerHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());


	if(HasAuthority())
	{
		SpawnWeapon(Combat.DefaultPrimaryWeaponClass, PrimaryWeapon, ("AR_1P_Socket"));
		SpawnWeapon(Combat.DefaultSecondaryWeaponClass, SecondaryWeapon, ("MG_1P_Socket"));
		WeaponArrayChecks();

		SpawnWeapon3P(Combat.PrimaryWeapon3PClass, Primary3PWeapon, ("AR_3P_Socket"));
		SpawnWeapon3P(Combat.SecondaryWeapon3PClass, Secondary3PWeapon, ("Pistol_3P_Holstered_Socket"));

	}


	// Sets the default settings of the character 
	DefaultCameraHeight = CameraComp->GetRelativeLocation().Z;
	DefaultCapsuleHeight = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	DefaultMeshHeight = GetMesh()->GetRelativeLocation().Z;

	// Sets the crouched settings of the character 
	CrouchedCameraHeight = DefaultCameraHeight * CrouchedCameraHeightMultiplier;
	CrouchedCapsuleHeight = DefaultCapsuleHeight * CrouchedCapsuleHeightMultiplier;
	CrouchedMeshHeight = DefaultMeshHeight / CrouchedMeshHeightMultiplier;

	//Sets the values of the crouch timeline
	if (FCrouchCurve)
	{
		CrouchTimeline->AddInterpFloat(FCrouchCurve, CrouchInterpFunction, FName("Alpha"));
		CrouchTimeline->SetLooping(false);
		CrouchTimeline->SetIgnoreTimeDilation(true);
	}

	HealthComp->OnHealthChanged.AddDynamic(this, &AC_PlayerCharacter::OnHealthChanged);
	HealthComp->OnShieldsChanged.AddDynamic(this, &AC_PlayerCharacter::OnSheildsChanged);

	OutOfCombatDelegate.BindUFunction(this, FName("UpdateCombatState"), ECombatState::DEFAULT);

	// FOV Setup

	DefaultFOV = CameraComp->FieldOfView;

	if(HasAuthority())
	{
		for (auto x : EquippedWeapon3PArray)
		{
			UE_LOG(LogTemp, Error, TEXT("ELEMENTs before switch -- 3P -- : %s"), *x->GetName());
		}

		for (auto i : EquippedWeaponArray)
		{
			UE_LOG(LogTemp, Error, TEXT("ELEMENTs befrore switch -- 1P -- : %s"), *i->GetName());
		}
	}
	

	
}

void AC_PlayerCharacter::Tick(float DeltaTime)

{
	Super::Tick(DeltaTime);

	Movement.PlayersVelocity = AC_PlayerCharacter::GetVelocity();
	Movement.PlayersVelocityMagnitude = Movement.PlayersVelocity.Size();

	float TargetFOV = bZoomIn ? ZoomedFOV : DefaultFOV;

	float NewFOV = FMath::FInterpTo(CameraComp->FieldOfView, TargetFOV, DeltaTime, ZoomInterpSpeed);
	CameraComp->SetFieldOfView(NewFOV);

}

// HEALTH SYSTEM

void AC_PlayerCharacter::OnHealthChanged(UC_HealthComponent* HealthComponent, float Health, bool bUpdateCombatState)
{
	HUD->HUDUpdateHealthImage(HealthComp->GetHealth());

	if(bUpdateCombatState)
	{
		if (HealthComp->GetHealth() <= 0.0f)
		{
			//UE_LOG(LogTemp, Error, TEXT("Player is DEAD"));
			GetWorldTimerManager().ClearAllTimersForObject(this);
		}

		else
		{
			UpdateCombatState(ECombatState::COMBAT);

			if (GetWorldTimerManager().IsTimerActive(OutOfCombatHandle))
			{
				GetWorldTimerManager().ClearTimer(OutOfCombatHandle);
			}

			GetWorld()->GetTimerManager().SetTimer(OutOfCombatHandle, OutOfCombatDelegate, 5.0f, false);
		}


	}
}

void AC_PlayerCharacter::OnSheildsChanged(UC_HealthComponent* HealthComponent, float Shields)
{
	//UE_LOG(LogTemp, Error, TEXT("Shields have changed"));
	HUD->HUDUpdateShieldBars(HealthComp->GetShields());
}

// MOVEMENT SYSTEM

bool AC_PlayerCharacter::CheckIdle()
{
	if (Movement.PlayersVelocityMagnitude > 0.0f)
	{
		return false;
	}

	else
	{
		return true;
	}
}

void AC_PlayerCharacter::MoveForward(float Axis)
{

	if (GetCharacterMovement()->IsFalling())
	{
		UpdateMovementSettings(EMovementState::JUMP);
	}

	else
	{
		// If crouch key is pressed, player is crouching otherwise they are walking
		bCrouchKeyDown ? UpdateMovementSettings(EMovementState::CROUCH) : UpdateMovementSettings(EMovementState::WALK);
	}

	FRotator Rotation = Controller->GetControlRotation();
	FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);

	// Gets the forward vector of the player and moves in the forwrads or backwards direction
	FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(Direction, Axis);
}

void AC_PlayerCharacter::MoveRight(float Axis)
{
	// Stops walking if we are in air
	if (GetCharacterMovement()->IsFalling())
	{
		UpdateMovementSettings(EMovementState::JUMP);
	}

	else
	{
		// If crouch key is pressed, player is crouching otherwise they are walking
		bCrouchKeyDown ? UpdateMovementSettings(EMovementState::CROUCH) : UpdateMovementSettings(EMovementState::WALK);
	}

	FRotator Rotation = Controller->GetControlRotation();
	FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);

	// Gets the left/right vector of the player and moves in the left or right direction
	FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(Direction, Axis);
}

void AC_PlayerCharacter::JumpStart()
{
	if(bCanJump && (MovementState == EMovementState::WALK || MovementState == EMovementState::SPRINT))
	{
		UpdateMovementSettings(EMovementState::JUMP);
		AC_PlayerCharacter::LaunchCharacter(Movement.JumpHeight, false, true);
	}
}

void AC_PlayerCharacter::Landed(const FHitResult& Hit)
{
	// Ensures player cannot spam jump
	GetWorldTimerManager().SetTimer(JumpCooldownHandle, this, &AC_PlayerCharacter::ResetJump, 0.5f, false);
}

void AC_PlayerCharacter::ResetJump()
{
	bCanJump = true;
	GetWorldTimerManager().ClearTimer(JumpCooldownHandle);
}

void AC_PlayerCharacter::StartCrouch()
{
	if (bCanCrouch && MovementState == EMovementState::WALK || MovementState == EMovementState::SPRINT)
	{
		UpdateMovementSettings(EMovementState::CROUCH);
		CrouchTimeline->Play();
		CameraManager->SetViewPitchAmount(-70.0f, 89.9f);
	}
}

void AC_PlayerCharacter::EndCrouch()
{
	bCrouchKeyDown = false;
	CrouchTimeline->Reverse();
	CameraManager->SetViewPitchAmount(-89.9f, 89.9f);
}

// Crouch Timeline Update
void AC_PlayerCharacter::CrouchTimelineFloatReturn(float Value)
{
	float x = CameraComp->GetRelativeLocation().X;
	float y = CameraComp->GetRelativeLocation().Y;

	// Smoothly transitions between Camera heights
	CameraComp->SetRelativeLocation(FVector(x, y, (FMath::Lerp(DefaultCameraHeight, CrouchedCameraHeight, Value))));

	// Smoothly transitions between capsule heights
	GetCapsuleComponent()->SetCapsuleHalfHeight(FMath::Lerp(DefaultCapsuleHeight, CrouchedCapsuleHeight, Value));

	MeshX = GetMesh()->GetRelativeLocation().X;
	MeshY = GetMesh()->GetRelativeLocation().Y;

	GetMesh()->SetRelativeLocation(FVector(MeshX, MeshY, (FMath::Lerp(DefaultMeshHeight, CrouchedMeshHeight, Value))));
}

void AC_PlayerCharacter::UpdateMovementSettings(EMovementState NewState)
{
	switch (NewState)
	{
	case EMovementState::WALK:

		GetCharacterMovement()->MaxWalkSpeed = 500.0f;
		bCanJump = true;
		bCanCrouch = true;
		//UE_LOG(LogTemp, Log, TEXT("Player is Walking"));
		break;

	case EMovementState::JUMP:

		bCanJump = false;
		bCanCrouch = false;
		//UE_LOG(LogTemp, Log, TEXT("Player is Jumping"));
		break;

	case EMovementState::CROUCH:

		GetCharacterMovement()->MaxWalkSpeed = 150.0f;
		bCrouchKeyDown = true;
		bCanJump = false;
		//UE_LOG(LogTemp, Log, TEXT("Player is Crouching"));
		break;

	default:
		//UE_LOG(LogTemp, Log, TEXT("Player is UNDEFINED"));
		break;
	}
}

void AC_PlayerCharacter::UpdateCombatState(ECombatState NewState)
{
	switch (NewState)
	{
	case ECombatState::DEFAULT:
		//UE_LOG(LogTemp, Log, TEXT("Player is DEFAULT!!!!!!!!"));
		HealthComp->StartHealthRegeneration();
		break;

	case ECombatState::COMBAT:
		//UE_LOG(LogTemp, Log, TEXT("Player is COMBAT!!!!!!!!"));
		HealthComp->StopShieldRegeneration();
		HealthComp->StopHealthRegeneration();
		break;
	}
}

// INTERACT SYSTEM

void AC_PlayerCharacter::Interact()
{

	ETraceTypeQuery Trace = UEngineTypes::ConvertToTraceType(COLLISION_PICKUP);

	TArray<AActor* > IgnoredActorsArray;
	IgnoredActorsArray.Add(this);

	FHitResult InteractHit;

	bool bHit = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), GetActorLocation(), GetActorLocation(), 150.0f, Trace,
		false, IgnoredActorsArray, EDrawDebugTrace::ForDuration, InteractHit, true, FLinearColor::Yellow, FLinearColor::Black, 3.0f);


	/*FVector StartLocation = CameraComp->GetComponentLocation();
	FVector EndLocation = StartLocation + (CameraComp->GetForwardVector() * 500.0f);*/ // FROM CENTRE OF CAMERA 

	if (bHit)
	{
		if(HasAuthority())
		{
			Multi_Interact(InteractHit);
		}

		else
		{
			Server_Interact(InteractHit);
		}
	}
}

void AC_PlayerCharacter::Server_Interact_Implementation(FHitResult Hit)
{
	Multi_Interact(Hit);
}

void AC_PlayerCharacter::Multi_Interact_Implementation(FHitResult Hit)
{
	// Execute the interact function in the interact actor
	if (Hit.GetActor()->GetClass()->ImplementsInterface(UC_InteractInterface::StaticClass()))
	{
		IC_InteractInterface::Execute_Interact(Hit.GetActor());
	}
}

// COMBAT SYSTEM 

// Spawn weapon in 1P
void AC_PlayerCharacter::SpawnWeapon(TSubclassOf<AC_BaseWeapon> WeaponClass, AC_BaseWeapon*& Weapon, FName WeaponSocket)
{
	//Spawn new weapon in first person
	FActorSpawnParameters SpawnParams;

	FTransform WeaponTransform = DefaultMesh->GetSocketTransform(WeaponSocket, ERelativeTransformSpace::RTS_World);
	FVector WeaponSpawnLocation = WeaponTransform.GetLocation();
	FRotator WeaponSpawnRotation = WeaponTransform.GetRotation().Rotator();

	Weapon = GetWorld()->SpawnActor<AC_BaseWeapon>(WeaponClass, WeaponSpawnLocation, WeaponSpawnRotation, SpawnParams); 
	Weapon->AttachToComponent(DefaultMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, WeaponSocket);
	Weapon->SetOwner(this);

	AC_BaseGun* Gun = Cast<AC_BaseGun>(Weapon);

	EquippedWeaponArray.Emplace(Weapon);
}

void AC_PlayerCharacter::WeaponArrayChecks()
{
	EquippedWeaponArray[0]->SetActorHiddenInGame(false);
	EquippedWeaponArray[1]->SetActorHiddenInGame(true);
}

void AC_PlayerCharacter::SwitchWeapons()
{
	if(bCanSwitch)
	{
		EndFire();

		EquippedWeaponArray.Swap(0, 1);
		WeaponArrayChecks();
		WeaponArray3PChecks();
		AC_BaseGun* Gun = Cast<AC_BaseGun>(EquippedWeaponArray[0]);

		if(Gun)
		{
			DefaultMesh->GetAnimInstance()->Montage_Play(Gun->GetWeaponEquipMontage(), 1.0f);

			bIsSwitching = !bIsSwitching;
			
			// Call Server RPC for clients 
			if (!HasAuthority())
			{
				Server_Switch(Gun->GetWeapon3PEquipMontage());
			}
			

			bCanZoom = false;
			bCanSwitch = false;
			bCanFire = false;
			bCanReload = false;

			UE_LOG(LogTemp, Log, TEXT("Weapon switch length is: %f"), Gun->WeaponStats.SwitchLength);

			GetWorldTimerManager().SetTimer(SwitchResetHandle, this, &AC_PlayerCharacter::ResetCanSwitch, Gun->WeaponStats.SwitchLength, false);
		}
	
		// If zoomed in while switching, stop zooming
		EndZoom();
	}
}

void AC_PlayerCharacter::ResetCanSwitch()
{
	bCanSwitch = true;
	bCanZoom = true;
	bCanReload = true;
	bCanFire = true;
}

void AC_PlayerCharacter::OnRep_Switch()
{
	AC_BaseGun* Gun = Cast<AC_BaseGun>(EquippedWeaponArray[0]);
	Mesh3P->GetAnimInstance()->Montage_Play(Gun->GetWeapon3PEquipMontage(), 1.0f);
}

void AC_PlayerCharacter::Server_Switch_Implementation(UAnimMontage* Montage)
{
	Mesh3P->GetAnimInstance()->Montage_Play(Montage, 1.0f);
}

void AC_PlayerCharacter::SpawnWeapon3P(TSubclassOf<AC_Weapon3P> WeaponClass, AC_Weapon3P*& Weapon, FName WeaponSocket)
{
	//Spawn new weapon in first person
	FActorSpawnParameters SpawnParams;

	FTransform WeaponTransform = Mesh3P->GetSocketTransform(WeaponSocket, ERelativeTransformSpace::RTS_World);
	FVector WeaponSpawnLocation = WeaponTransform.GetLocation();
	FRotator WeaponSpawnRotation = WeaponTransform.GetRotation().Rotator();

	Weapon = GetWorld()->SpawnActor<AC_Weapon3P>(WeaponClass, WeaponSpawnLocation, WeaponSpawnRotation, SpawnParams);
	Weapon->AttachToComponent(Mesh3P, FAttachmentTransformRules::SnapToTargetIncludingScale, WeaponSocket);
	Weapon->SetOwner(this);

	EquippedWeapon3PArray.Emplace(Weapon);
}

void AC_PlayerCharacter::WeaponArray3PChecks()
{
	// Changes the skeletal mesh's of the 3P weapons using a server RPC as SetSkeletalMesh does not replicate properly
	Server_ChangeSkeletalMesh(EquippedWeaponArray[0]->WeaponMesh->SkeletalMesh, EquippedWeapon3PArray[0]->WeaponMesh3P);
	Server_ChangeSkeletalMesh(EquippedWeaponArray[1]->WeaponMesh->SkeletalMesh, EquippedWeapon3PArray[1]->WeaponMesh3P);

	if(HasAuthority())
	{
		// Updates weapon type to currently equipped weapon, this will update the ABP and location of DefaultMesh
		WeaponType = EquippedWeaponArray[0]->Type;
		OnWeaponTypeUpdate();

		EquippedWeapon3PArray[0]->AttachToComponent(Mesh3P, FAttachmentTransformRules::SnapToTargetIncludingScale, EquippedWeaponArray[0]->Socket3P);
		EquippedWeapon3PArray[1]->AttachToComponent(Mesh3P, FAttachmentTransformRules::SnapToTargetIncludingScale, EquippedWeaponArray[1]->Socket3PHolstered);
	}

	else
	{
		Server_WeaponArray3PChecks(EquippedWeaponArray[0]->Type, EquippedWeaponArray[0]->Socket3P, EquippedWeaponArray[1]->Socket3PHolstered);
	}
}

void AC_PlayerCharacter::Server_WeaponArray3PChecks_Implementation(EWeaponType NewType, FName Socket3P, FName Socket3PHolstered)
{
	WeaponType = NewType;
	OnWeaponTypeUpdate(); // Does not replicate for clients 

	EquippedWeapon3PArray[0]->AttachToComponent(Mesh3P, FAttachmentTransformRules::SnapToTargetIncludingScale, Socket3P);
	EquippedWeapon3PArray[1]->AttachToComponent(Mesh3P, FAttachmentTransformRules::SnapToTargetIncludingScale, Socket3PHolstered);
}

void AC_PlayerCharacter::Server_ChangeSkeletalMesh_Implementation(USkeletalMesh* SKMesh, USkinnedMeshComponent* SkinnedMesh)
{
	if(HasAuthority())
	{
		Multi_ChangeSkeletalMesh(SKMesh, SkinnedMesh);
		SkinnedMesh->SetSkeletalMesh(SKMesh, false);
	}
}

void AC_PlayerCharacter::Multi_ChangeSkeletalMesh_Implementation(USkeletalMesh* SKMesh, USkinnedMeshComponent* SkinnedMesh)
{
	if(!HasAuthority())
	{
		SkinnedMesh->SetSkeletalMesh(SKMesh, false);
	}
}

void AC_PlayerCharacter::Reload()
{
	
	if(EquippedWeaponArray[0])
	{
		AC_BaseGun* Gun = Cast<AC_BaseGun>(EquippedWeaponArray[0]);
		if(Gun)
		{
			if(Gun->WeaponStats.CurrentAmmo != Gun->WeaponStats.MaxMagazineAmmo && Gun->WeaponStats.MaxReservesAmmo != 0 && bCanReload)
			{
				// The OnRep only works on server 
				bIsReloading = !bIsReloading;
				
				// Call Server RPC for clients 
				if (!HasAuthority())
				{
					Server_Reload(Gun->GetWeapon3PReloadMontage());
				}

			
				Gun->Reload();
				DefaultMesh->GetAnimInstance()->Montage_Play(Gun->GetWeaponReloadMontage(), 1.0f);
				

				bCanReload = false;
				bCanSwitch = false;
				bCanZoom = false;

				// If we are zoomed in when reload starts
				EndZoom();

				// Allows for zooming and switching after reload
				GetWorldTimerManager().SetTimer(ReloadResetHandle, this, &AC_PlayerCharacter::ResetCanReload, Gun->WeaponStats.ReloadLength, false);
				
			}
		}
	}
}

void AC_PlayerCharacter::OnRep_Reload()
{
	AC_BaseGun* Gun = Cast<AC_BaseGun>(EquippedWeaponArray[0]);
	Mesh3P->GetAnimInstance()->Montage_Play(Gun->GetWeapon3PReloadMontage(), 1.0f);
}

void AC_PlayerCharacter::Server_Reload_Implementation(UAnimMontage* Montage)
{
	Mesh3P->GetAnimInstance()->Montage_Play(Montage, 1.0f);
}

void AC_PlayerCharacter::ResetCanReload()
{
	bCanReload = true;
	bCanSwitch = true;
	bCanZoom = true;
}

void AC_PlayerCharacter::OnWeaponTypeUpdate()
{
	switch (WeaponType)
	{
	case EWeaponType::RIFLE:
		DefaultMesh->SetRelativeLocation(FVector(0,0, -175.0));
		break;

	case EWeaponType::PISTOL:
		DefaultMesh->SetRelativeLocation(FVector(0, 0, -170.0));
		break;

	}
}

// REPLICATION TESTING

void AC_PlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AC_PlayerCharacter, PrimaryWeapon);
	DOREPLIFETIME(AC_PlayerCharacter, SecondaryWeapon);
	DOREPLIFETIME(AC_PlayerCharacter, Primary3PWeapon);
	DOREPLIFETIME(AC_PlayerCharacter, Secondary3PWeapon);

	DOREPLIFETIME(AC_PlayerCharacter, EquippedWeaponArray);
	DOREPLIFETIME(AC_PlayerCharacter, EquippedWeapon3PArray);
	DOREPLIFETIME(AC_PlayerCharacter, CombatState);
	DOREPLIFETIME(AC_PlayerCharacter, WeaponType);

	DOREPLIFETIME(AC_PlayerCharacter, bIsReloading);
	DOREPLIFETIME(AC_PlayerCharacter, bIsFiring);
	DOREPLIFETIME(AC_PlayerCharacter, bStopFiring);
	DOREPLIFETIME(AC_PlayerCharacter, bIsSwitching);



	//DOREPLIFETIME(AC_PlayerCharacter, Combat);

}

///////////////////////////////////////

// organise 
FVector AC_PlayerCharacter::GetPawnViewLocation() const
{
	return CameraComp->GetComponentLocation();
}

void AC_PlayerCharacter::StartFire()
{
	if (EquippedWeaponArray[0] && bCanFire)
	{
		EquippedWeaponArray[0]->Attack();
	}
}

void AC_PlayerCharacter::EndFire()
{
	if (EquippedWeaponArray[0])
	{
		EquippedWeaponArray[0]->StopAttack();
	}
}

void AC_PlayerCharacter::OnRep_Fire()
{
	AC_BaseGun* Gun = Cast<AC_BaseGun>(EquippedWeaponArray[0]);
	Mesh3P->GetAnimInstance()->Montage_Play(Gun->GetWeapon3PFireMontage(), 1.0f);
}

void AC_PlayerCharacter::Server_Fire_Implementation(UAnimMontage* Montage)
{
	Mesh3P->GetAnimInstance()->Montage_Play(Montage, 1.0f);
}

void AC_PlayerCharacter::OnRep_StopFire()
{
	AC_BaseGun* Gun = Cast<AC_BaseGun>(EquippedWeaponArray[0]);
	Mesh3P->GetAnimInstance()->Montage_Stop(0.1f, Gun->GetWeapon3PFireMontage());
}

void AC_PlayerCharacter::Server_StopFire_Implementation(UAnimMontage* Montage)
{
	Mesh3P->GetAnimInstance()->Montage_Stop(0.1f, Montage);
}

void AC_PlayerCharacter::OnWeaponFire()
{
	bIsFiring = !bIsFiring;

	AC_BaseGun* Gun = Cast<AC_BaseGun>(EquippedWeaponArray[0]);

	if(Gun)
	{
		// Call Server RPC for clients 
		if (!HasAuthority())
		{
			Server_Fire(Gun->GetWeapon3PFireMontage());
		}
	}
}

void AC_PlayerCharacter::OnWeaponStopFire()
{
	bStopFiring = !bStopFiring;

	AC_BaseGun* Gun = Cast<AC_BaseGun>(EquippedWeaponArray[0]);
	if(Gun)
	{
		if (!HasAuthority())
		{
			Server_StopFire(Gun->GetWeapon3PFireMontage());
		}
	}
}


void AC_PlayerCharacter::BeginZoom()
{
	if (EquippedWeaponArray[0])
	{
		AC_BaseGun* Gun = Cast<AC_BaseGun>(EquippedWeaponArray[0]);
		if (Gun)
		{
			// If weapon has zoom and if player can zoom
			if (Gun->WeaponStats.bCanZoom && bCanZoom)
			{
				bZoomIn = true;
			}
		}
	}
	
}

void AC_PlayerCharacter::EndZoom()
{
	bZoomIn = false;
}

void AC_PlayerCharacter::PlayMontage(UAnimMontage* Montage)
{
	DefaultMesh->GetAnimInstance()->Montage_Play(Montage, 1.0f);
}

void AC_PlayerCharacter::StopMontage(UAnimMontage* Montage)
{
	DefaultMesh->GetAnimInstance()->Montage_Stop(1.0f, Montage);
}

// INPUT

void AC_PlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// MOVING
	PlayerInputComponent->BindAxis("MoveForward", this, &AC_PlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AC_PlayerCharacter::MoveRight);

	// LOOKING
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookRight", this, &APawn::AddControllerYawInput);

	// JUMP

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AC_PlayerCharacter::JumpStart);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AC_PlayerCharacter::StartCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AC_PlayerCharacter::EndCrouch);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AC_PlayerCharacter::Interact);

	// COMBAT

	PlayerInputComponent->BindAction("SwitchWeapons", IE_Pressed, this, &AC_PlayerCharacter::SwitchWeapons);


	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AC_PlayerCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AC_PlayerCharacter::EndFire);

	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &AC_PlayerCharacter::BeginZoom);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &AC_PlayerCharacter::EndZoom);

	PlayerInputComponent->BindAction("Reload", IE_Pressed,  this, &AC_PlayerCharacter::Reload);
}