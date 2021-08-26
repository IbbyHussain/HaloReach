
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
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/SpringArmComponent.h"
#include "HaloReach/UI/HUD/C_PlayerHUDWidget.h"

#include "HaloReach/Libraries/C_SpawnLibrary.h"

#include "GameFramework/CharacterMovementComponent.h"


AC_PlayerCharacter::AC_PlayerCharacter(const FObjectInitializer& ObjectInitializer) : 
	Super(ObjectInitializer.SetDefaultSubobjectClass<UC_PlayerCMC>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	WeaponAttachSocketName = ("R_GunSocket");

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComp->SetupAttachment(GetCapsuleComponent());
	//CameraComp->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	CameraComp->bUsePawnControlRotation = true;
	CameraComp->SetActive(true);

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
	Mesh3P->SetOwnerNoSee(true);
	Mesh3P->SetupAttachment(GetCapsuleComponent());
	// Use this mesh's shadows
	Mesh3P->bCastDynamicShadow = true;
	Mesh3P->CastShadow = true;

	HealthComp = CreateDefaultSubobject<UC_HealthComponent>(TEXT("HealthComponent"));

	//Crouch Timeline
	CrouchTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("CrouchTimeline"));
	CrouchInterpFunction.BindUFunction(this, FName("CrouchTimelineFloatReturn"));

	// Death Camera components

	DeathSpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("Death SpringArm"));
	DeathSpringArmComp->SetupAttachment(RootComponent);
	DeathSpringArmComp->TargetArmLength = 500.0f;

	DeathCameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Death Camera"));
	DeathCameraComp->SetupAttachment(DeathSpringArmComp, USpringArmComponent::SocketName);

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

	bIsFiring = false;

	bCanMelee = true;

	DefaultSpeed = 500.0f;
	CrouchSpeed = 150.0f;
	
	ActorsIgnored = { this };
	IgnoredActorsTracking = { this };

	MeleeTrackTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("Melee Track Timeline"));

	MeleeTrackInterpFunction.BindUFunction(this, FName("MeleeTrackTimelineFloatReturn"));
	MeleeTrackTimelineFinished.BindUFunction(this, FName("OnMeleeTrackTimelineFinished"));

}

void AC_PlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	CameraManager = Cast<AC_PlayerCameraManager>(UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0));

	HUD = Cast<AC_PlayerHUD>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD());
	//HUD = Cast<AC_PlayerHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());

	GetPlayerMovementComponent()->SetPlayerSpeed(DefaultSpeed);

	if(HasAuthority())
	{
		// Spawn weapons for 1P

		PrimaryWeapon = UC_SpawnLibrary::SpawnActorAtSocket<AC_BaseWeapon>(GetWorld(), Combat.DefaultPrimaryWeaponClass, PrimaryWeapon, DefaultMesh, ("AR_1P_Socket"));
		PrimaryWeapon->SetOwner(this);
		EquippedWeaponArray.Emplace(PrimaryWeapon);

		SecondaryWeapon = UC_SpawnLibrary::SpawnActorAtSocket<AC_BaseWeapon>(GetWorld(), Combat.DefaultSecondaryWeaponClass, SecondaryWeapon, DefaultMesh, ("MG_1P_Socket"));
		SecondaryWeapon->SetOwner(this);
		EquippedWeaponArray.Emplace(SecondaryWeapon);

		WeaponArrayChecks();

		// Spawn weapons for 3P

		Primary3PWeapon = UC_SpawnLibrary::SpawnActorAtSocket<AC_Weapon3P>(GetWorld(), Combat.PrimaryWeapon3PClass, Primary3PWeapon, Mesh3P, ("AR_3P_Socket"));
		Primary3PWeapon->SetOwner(this);
		EquippedWeapon3PArray.Emplace(Primary3PWeapon);

		Secondary3PWeapon = UC_SpawnLibrary::SpawnActorAtSocket<AC_Weapon3P>(GetWorld(), Combat.SecondaryWeapon3PClass, Secondary3PWeapon, Mesh3P, ("Pistol_3P_Holstered_Socket"));
		Secondary3PWeapon->SetOwner(this);
		EquippedWeapon3PArray.Emplace(Secondary3PWeapon);
	}


	// Sets the default settings of the character 
	DefaultCameraHeight = CameraComp->GetRelativeLocation().Z;
	DefaultCapsuleHeight = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	DefaultMeshHeight = Mesh3P->GetRelativeLocation().Z;

	// Sets the crouched settings of the character 
	CrouchedCameraHeight = DefaultCameraHeight * CrouchedCameraHeightMultiplier;
	CrouchedCapsuleHeight = DefaultCapsuleHeight * CrouchedCapsuleHeightMultiplier;
	//CrouchedMeshHeight = 0; //DefaultMeshHeight + 45.0f;

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

	if (FMeleeTrackCurve)
	{
		// Now we set the functions and some values.
		MeleeTrackTimeline->AddInterpFloat(FMeleeTrackCurve, MeleeTrackInterpFunction, FName("Bravo"));
		MeleeTrackTimeline->SetTimelineFinishedFunc(MeleeTrackTimelineFinished);
		MeleeTrackTimeline->SetLooping(false);
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

	SetControlRotation();
}

void AC_PlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	PlayerCMC = Cast<UC_PlayerCMC>(Super::GetMovementComponent());
}

// HEALTH SYSTEM

void AC_PlayerCharacter::OnHealthChanged(UC_HealthComponent* HealthComponent, float Health, bool bUpdateCombatState)
{
	HUD->HUDUpdateHealthImage(HealthComp->GetHealth());

	if(bUpdateCombatState)
	{
		// bIsDead so that death() is only called once
		if (HealthComp->GetHealth() <= 0.0f && !bIsDead)
		{
			Death();
			//BPDeath();
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
	if (!bIsDead) 
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
}

void AC_PlayerCharacter::MoveRight(float Axis)
{
	if(!bIsDead)
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
}

// JUMP SYSYTEM

void AC_PlayerCharacter::JumpStart()
{
	if(bCanJump && (MovementState == EMovementState::WALK || MovementState == EMovementState::SPRINT))
	{
		if(HasAuthority())
		{
			UpdateMovementSettings(EMovementState::JUMP);
			AC_PlayerCharacter::LaunchCharacter(Movement.JumpHeight, false, true);
		}

		else
		{
			Server_Jump();
		}
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

void AC_PlayerCharacter::Server_Jump_Implementation()
{
	// Launch character requires to be executed on server
	UpdateMovementSettings(EMovementState::JUMP);
	AC_PlayerCharacter::LaunchCharacter(Movement.JumpHeight, false, true);
}

// CROUCH SYSTEM

void AC_PlayerCharacter::StartCrouch()
{
	if (bCanCrouch && MovementState == EMovementState::WALK || MovementState == EMovementState::SPRINT)
	{
		UpdateMovementSettings(EMovementState::CROUCH);
		CameraManager->SetViewPitchAmount(-70.0f, 89.9f);
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), CrouchStartSound, GetActorLocation());
		CrouchTimeline->Play();
	}
}

void AC_PlayerCharacter::Server_CrouchTimeline_Implementation(bool bCrouch)
{
	bCrouchKeyDown = bCrouch;
}

void AC_PlayerCharacter::Server_CrouchSpeed_Implementation()
{
	GetPlayerMovementComponent()->SetPlayerSpeed(CrouchSpeed);
}

void AC_PlayerCharacter::SetCrouchKeyDown(bool bCrouch)
{
	if (HasAuthority())
	{
		bCrouchKeyDown = bCrouch;
	}

	else
	{
		Server_CrouchTimeline(bCrouch);
	}
}

void AC_PlayerCharacter::EndCrouch()
{
	CrouchTimeline->Reverse();
	SetCrouchKeyDown(false);
	bCrouchKeyDown = false;
	CameraManager->SetViewPitchAmount(-89.9f, 89.9f);
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), CrouchEndSound, GetActorLocation());
}

// Crouch Timeline Update
void AC_PlayerCharacter::CrouchTimelineFloatReturn(float Value)
{
	float x = CameraComp->GetRelativeLocation().X;
	float y = CameraComp->GetRelativeLocation().Y;

	MeshX = Mesh3P->GetRelativeLocation().X;
	MeshY = Mesh3P->GetRelativeLocation().Y;
	
	if (HasAuthority())
	{
		Multi_Crouch(DefaultCapsuleHeight, CrouchedCapsuleHeight, DefaultMeshHeight, CrouchedMeshHeight, Value);
	}

	else
	{
		Server_Crouch(DefaultCapsuleHeight, CrouchedCapsuleHeight, DefaultMeshHeight, CrouchedMeshHeight, Value);
	}

	// Smoothly transitions between Camera heights
	CameraComp->SetRelativeLocation(FVector(x, y, (FMath::Lerp(DefaultCameraHeight, CrouchedCameraHeight, Value))));
}

void AC_PlayerCharacter::Server_Crouch_Implementation(float Height, float NewHeight, float MeshHeight, float NewMeshHeight, float Alpha)
{
	Multi_Crouch(Height, NewHeight, MeshHeight, NewMeshHeight, Alpha);
}

void AC_PlayerCharacter::Multi_Crouch_Implementation(float Height, float NewHeight, float MeshHeight, float NewMeshHeight, float Alpha)
{
	// Ensures player mesh is not below the ground
	GetCapsuleComponent()->SetCapsuleHalfHeight(FMath::Lerp(96.0f, 50.0f, Alpha));

	float x = FMath::Lerp(96.0f, 50.0f, Alpha) * -1.0f;
	Mesh3P->SetRelativeLocation(FVector(0.0f, 0.0f, x));
}

void AC_PlayerCharacter::UpdateMovementSettings(EMovementState NewState)
{
	switch (NewState)
	{
	case EMovementState::WALK:

		GetPlayerMovementComponent()->SetPlayerSpeed(DefaultSpeed);
		bCanJump = true;
		bCanCrouch = true;

		
		break;

	case EMovementState::JUMP:

		bCanJump = false;
		bCanCrouch = false;

		
		break;

	case EMovementState::CROUCH:

		GetPlayerMovementComponent()->SetPlayerSpeed(CrouchSpeed);
		SetCrouchKeyDown(true);

		bCanJump = false;

		/*bCanFire = true;
		bCanReload = true;
		bCanZoom = true;
		bCanMelee = true;
		bCanSwitch = true;*/
		break;

	case EMovementState::IDLE:

		bCanJump = false;
		bCanCrouch = false;
		bCanFire = false;
		bCanReload = false;
		bCanZoom = false;
		bCanMelee = false;
		bCanSwitch = false;
		break;

	default:
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

# pragma region  Weapon Switching

void AC_PlayerCharacter::SwitchWeapons()
{
	if(bCanSwitch)
	{
		EndFire();

		EquippedWeaponArray.Swap(0, 1);
		WeaponArrayChecks();
		WeaponArray3PChecks();

		AC_BaseGun* Gun = Cast<AC_BaseGun>(EquippedWeaponArray[0]);
		AC_BaseGun* Gun2 = Cast<AC_BaseGun>(EquippedWeaponArray[1]);

		HUD->UpdateWeaponReserves();

		if(Gun)
		{
			Gun2->StopReturnTimeline();

			DefaultMesh->GetAnimInstance()->Montage_Play(Gun->GetWeaponEquipMontage(), 1.0f);

			// Call Server RPC for clients 
			if(HasAuthority())
			{
				Multi_PlayMontage(Mesh3P, Gun->GetWeapon3PEquipMontage());
			}

			else
			{
				Server_PlayMontage(Mesh3P, Gun->GetWeapon3PEquipMontage());
			}
			

			bCanZoom = false;
			bCanSwitch = false;
			bCanFire = false;
			bCanReload = false;
			bCanMelee = false;

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
	bCanMelee = true;
}

# pragma endregion

void AC_PlayerCharacter::WeaponArrayChecks()
{
	EquippedWeaponArray[0]->SetActorHiddenInGame(false);
	EquippedWeaponArray[1]->SetActorHiddenInGame(true);
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
		GEngine->AddOnScreenDebugMessage(-1, 4.5f, FColor::Purple, __FUNCTION__);
		if(Gun)
		{
			if(Gun->WeaponStats.CurrentAmmo != Gun->WeaponStats.MaxMagazineAmmo && Gun->WeaponStats.CurrentReservesAmmo != 0 && bCanReload)
			{
				// Restrictions 
				// stops firing if, reload at same time
				EndFire();
				EndZoom();

				// Call Server RPC for clients 
				if (HasAuthority())
				{
					Multi_PlayMontage(Mesh3P, Gun->GetWeapon3PReloadMontage());
				}

				else
				{
					Server_PlayMontage(Mesh3P, Gun->GetWeapon3PReloadMontage());
					Server_GunReload(Gun);
				}

				GunReload(Gun);

				PlayMontage(DefaultMesh, Gun->GetWeaponReloadMontage());
				
				bCanReload = false;
				bCanFire = false;
				bCanSwitch = false;
				bCanZoom = false;
				bCanMelee = false;

				// If we are zoomed in when reload starts
				EndZoom();

				// Allows for zooming and switching after reload
				GetWorldTimerManager().SetTimer(ReloadResetHandle, this, &AC_PlayerCharacter::ResetCanReload, Gun->WeaponStats.ReloadLength, false);
				
			}
		}
	}
}

void AC_PlayerCharacter::Server_PlayMontage_Implementation(USkeletalMeshComponent* MeshComp, UAnimMontage* Montage)
{
	Multi_PlayMontage(MeshComp, Montage);
}

void AC_PlayerCharacter::Multi_PlayMontage_Implementation(USkeletalMeshComponent* MeshComp, UAnimMontage* Montage)
{
	PlayMontage(MeshComp, Montage);
}

void AC_PlayerCharacter::ResetCanReload()
{
	bCanReload = true;
	bCanSwitch = true;
	bCanFire = true;
	bCanZoom = true;
	bCanMelee = true;
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

void AC_PlayerCharacter::UpdateReserveAmmo()
{
	HUD->UpdateWeaponReserves();
}

void AC_PlayerCharacter::GunReload(AC_BaseGun* Gun)
{
	Gun->Reload();
}

void AC_PlayerCharacter::Server_GunReload_Implementation(AC_BaseGun* Gun)
{
	GunReload(Gun);
}

# pragma region Melee Attack

void AC_PlayerCharacter::StartMelee()
{
	ClearActorsIgnoredArray();

	AC_BaseWeapon* Weapon = EquippedWeaponArray[0];

	if (Weapon)
	{
		PlayMontage(DefaultMesh, Weapon->GetWeapon1PMeleeMontage()); // 1p

		if (HasAuthority())
		{
			Multi_PlayMontage(Mesh3P, Weapon->GetWeapon3PMeleeMontage());
		}

		else
		{
			Server_PlayMontage(Mesh3P, Weapon->GetWeapon3PMeleeMontage());
		}

		GetWorldTimerManager().SetTimer(MeleeHandle, this, &AC_PlayerCharacter::ResetMelee, Weapon->MeleeTime, false);
	}
}

void AC_PlayerCharacter::ResetMelee()
{
	bCanMelee = true;
	bCanZoom = true;
	bCanSwitch = true;
	bCanFire = true;
	bCanReload = true;
}

void AC_PlayerCharacter::ClearActorsIgnoredArray()
{
	/*for(int i = ActorsIgnored.Num() - 1; i >= 0; i--)
	{
		AC_PlayerCharacter* Player = Cast<AC_PlayerCharacter>(ActorsIgnored[i]);
		if (Player)
		{
			ActorsIgnored.RemoveAt(i); // can be used to remove eleents from array in loop[
		}
	}*/

	ActorsIgnored.Empty();
	ActorsIgnored.Emplace(this);
}

void AC_PlayerCharacter::MeleeAttack(USkeletalMeshComponent* MeshComp)
{
	// Get the start and end location of the sphere trace (two sockets that are the length of the sword)
	FVector StartLocation = EquippedWeaponArray[0]->WeaponMesh->GetSocketLocation(EquippedWeaponArray[0]->MeleeStartSocket);
	FVector EndLocation = EquippedWeaponArray[0]->WeaponMesh->GetSocketLocation(EquippedWeaponArray[0]->MeleeEndSocket);
	
	FHitResult HitResult;

	// Convert the collision type to standard collision channel
	ETraceTypeQuery Trace6 = UEngineTypes::ConvertToTraceType(ECollisionChannel::COLLISION_MELEEDAMAGE);

	bool bHit = UKismetSystemLibrary::SphereTraceSingle(DefaultMesh, StartLocation, EndLocation, 20.0f, Trace6, false, ActorsIgnored, EDrawDebugTrace::None, HitResult, true, FLinearColor::Red, FLinearColor::Green);

	// Check if hit player
	AC_PlayerCharacter* HitPlayer = Cast<AC_PlayerCharacter>(HitResult.GetActor());

	if (bHit && HitPlayer)
	{
		if (!HasAuthority())
		{
			Server_MeleeAttack(HitPlayer);
		}

		if(HitPlayer->HealthComp->GetShields() <= 0.0f)
		{
			// Will one hit enemy player if sheields are 0
			UGameplayStatics::ApplyDamage(HitPlayer, HitPlayer->HealthComp->MaxHealth, UGameplayStatics::GetPlayerController(this, 0), this, NULL);
		}

		else
		{
			// will one hit shields, but does not allow for damage to affect health
			UGameplayStatics::ApplyDamage(HitPlayer, HitPlayer->HealthComp->GetShields(), UGameplayStatics::GetPlayerController(this, 0), this, NULL);
		}

		// Stops player from being damaged multiple times
		ActorsIgnored.Emplace(HitPlayer);
	}
}

void AC_PlayerCharacter::Server_MeleeAttack_Implementation(AC_PlayerCharacter* HitActor)
{
	if (HitActor->HealthComp->GetShields() <= 0.0f)
	{
		// Will one hit enemy player if sheields are 0
		UGameplayStatics::ApplyDamage(HitActor, HitActor->HealthComp->MaxHealth, UGameplayStatics::GetPlayerController(this, 0), this, NULL);
	}

	else
	{
		// will one hit shields
		UGameplayStatics::ApplyDamage(HitActor, HitActor->HealthComp->GetShields(), UGameplayStatics::GetPlayerController(this, 0), this, NULL);
	}
}

// called on input binding
void AC_PlayerCharacter::MeleeTracking()
{
	if (bCanMelee)
	{
		// Restrictions 
		EndFire();
		EndZoom();

		bCanMelee = false;
		bCanZoom = false;
		bCanSwitch = false;
		bCanFire = false;
		bCanReload = false;

		EnemyMap.Empty();

		FVector StartLocation = DefaultMesh->GetSocketLocation(MeleeStartSocket);
		FVector EndLocation = (GetActorRotation().Vector().ForwardVector * 0.0f) + StartLocation;
		FVector HalfSize = FVector(75.0f, 25.0f, 200.0f);
		FRotator Orientation = GetActorRotation();

		ETraceTypeQuery BoxTrace = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility);

		TArray<FHitResult> Hits;

		bool bHit = UKismetSystemLibrary::BoxTraceMulti(GetWorld(), StartLocation, EndLocation, HalfSize, Orientation, BoxTrace, false, IgnoredActorsTracking, EDrawDebugTrace::ForDuration, Hits, true, FLinearColor::Blue, FLinearColor::Black);

		for (auto x : Hits)
		{
			AC_PlayerCharacter* Enemy = Cast<AC_PlayerCharacter>(x.GetActor());

			if (Enemy)
			{
				FVector PlayerLocation = GetActorLocation();
				FVector EnemyLoc = Enemy->GetActorLocation();

				float Distance = (EnemyLoc - PlayerLocation).Size();

				EnemyMap.Emplace(Distance, Enemy);
			}
		}

		if (EnemyMap.Num() > 0)
		{
			TArray<float> DistancePlayer;
			EnemyMap.GenerateKeyArray(DistancePlayer);

			// Finds the smallest value in the array, in this case the shortest distance
			ShortestDistance = DistancePlayer[0]; 
			for (int i = 0; i < DistancePlayer.Num(); i++) 
			{
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Value: %f"), DistancePlayer[i])); //DistancesToPlayer[i]

				if (DistancePlayer[i] < ShortestDistance) 
				{
					ShortestDistance = DistancePlayer[i];
				}
			}

			// 150 is melee range for nomral melee to hit
			if (ShortestDistance > 150.0f)
			{
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("TRACKING MELEE ATTACK, SHORTEST Distance: %f"), ShortestDistance));
				MeleeTrackTimeline->PlayFromStart();
			}

			else
			{
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("NORMAL MELEE ATTACK, SHORTEST Distance: %f"), ShortestDistance));
				StartMelee();
			}
		}

		else
		{
			StartMelee();
		}
	}
}

void AC_PlayerCharacter::MeleeTrackTimelineFloatReturn(float Value)
{
	PlayerLoc = GetActorLocation();

	LocX = PlayerLoc.X;
	LocY = PlayerLoc.Y;
	LocZ = PlayerLoc.Z;

	AC_PlayerCharacter* ClosestEnemy;
	ClosestEnemy = *(EnemyMap.Find(ShortestDistance));

	EnemyLocation = ClosestEnemy->GetActorLocation();

	if(!HasAuthority())
	{
		Server_StartMelee(PlayerLoc, EnemyLocation, Value);
	}

	SetActorLocation(FMath::Lerp(PlayerLoc, EnemyLocation, Value));

	UKismetMathLibrary::FindLookAtRotation(PlayerLoc, EnemyLocation);
	

	if(bIsOverlappingEnemy(ClosestEnemy))
	{
		StopMeleeTrackTimeline();
		StartMelee();
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, FString::Printf(TEXT("COLLIDED")));
	}

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC)
	{
		PC->SetControlRotation(FMath::Lerp(GetActorRotation(), UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), EnemyLocation), Value));
		//PC->SetControlRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), EnemyLocation));
	}
}

// Set Actor location is server only
void AC_PlayerCharacter::Server_StartMelee_Implementation(FVector StartLocation, FVector EndLocation, float Alpha)
{
	SetActorLocation(FMath::Lerp(StartLocation, EndLocation, Alpha));
}

void AC_PlayerCharacter::OnMeleeTrackTimelineFinished()
{
	StartMelee();
}

bool AC_PlayerCharacter::bIsOverlappingEnemy(AC_PlayerCharacter* Enemy)
{
	if(GetCapsuleComponent()->IsOverlappingActor(Enemy))
	{
		return true;
	}

	else
	{
		return false;
	}
}

void AC_PlayerCharacter::StopMeleeTrackTimeline()
{
	if(MeleeTrackTimeline->IsPlaying())
	{
		MeleeTrackTimeline->Stop();
	}
}

# pragma endregion

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

	DOREPLIFETIME(AC_PlayerCharacter, bIsFiring);
	DOREPLIFETIME(AC_PlayerCharacter, bStopFiring);
	DOREPLIFETIME(AC_PlayerCharacter, bCrouchKeyDown);

	/*DOREPLIFETIME(AC_PlayerCharacter, DefaultMeshHeight);
	DOREPLIFETIME(AC_PlayerCharacter, CrouchedMeshHeight);

	DOREPLIFETIME(AC_PlayerCharacter, DefaultCameraHeight);
	DOREPLIFETIME(AC_PlayerCharacter, CrouchedCameraHeight);

	DOREPLIFETIME(AC_PlayerCharacter, DefaultCapsuleHeight);
	DOREPLIFETIME(AC_PlayerCharacter, CrouchedCapsuleHeight);*/

	DOREPLIFETIME_CONDITION(AC_PlayerCharacter, ControlRotation, COND_SkipOwner);
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

void AC_PlayerCharacter::OnRep_StopFire()
{
	//AC_BaseGun* Gun = Cast<AC_BaseGun>(EquippedWeaponArray[0]);
	//Mesh3P->GetAnimInstance()->Montage_Stop(0.1f, Gun->GetWeapon3PFireMontage());
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
			//Server_Fire(Gun->GetWeapon3PFireMontage());
		}
	}
}

void AC_PlayerCharacter::OnRep_Fire()
{
	//AC_BaseGun* Gun = Cast<AC_BaseGun>(EquippedWeaponArray[0]);
	//Mesh3P->GetAnimInstance()->Montage_Play(Gun->GetWeapon3PFireMontage(), 1.0f);
}

void AC_PlayerCharacter::Server_Fire_Implementation(UAnimMontage* Montage)
{
	Mesh3P->GetAnimInstance()->Montage_Play(Montage, 1.0f);
}

void AC_PlayerCharacter::OnWeaponStopFire()
{
	bStopFiring = !bStopFiring;

	AC_BaseGun* Gun = Cast<AC_BaseGun>(EquippedWeaponArray[0]);
	if(Gun)
	{
		if (!HasAuthority())
		{
			//Server_StopFire(Gun->GetWeapon3PFireMontage());
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

				HUD->HideHUDWidget();
				HUD->CreateZoomWidget(Gun->ZoomWidgetClass);
			}
		}
	}
	
}

void AC_PlayerCharacter::EndZoom()
{
	bZoomIn = false;

	if(!bIsDead)
	{
		HUD->ShowHUDWidget();
		HUD->DestroyZoomWidget();
	}
}

void AC_PlayerCharacter::PlayMontage(USkeletalMeshComponent* MeshComp, UAnimMontage* Montage)
{
	MeshComp->GetAnimInstance()->Montage_Play(Montage, 1.0f);
}

void AC_PlayerCharacter::StopMontage(USkeletalMeshComponent* MeshComp, UAnimMontage* Montage)
{
	MeshComp->GetAnimInstance()->Montage_Stop(1.0f, Montage);
}

void AC_PlayerCharacter::SetControlRotation()
{
	if(HasAuthority() || IsLocallyControlled())
	{
		ControlRotation = GetController()->GetControlRotation();
	}
}

# pragma region PlayerDeath

void AC_PlayerCharacter::Death()
{
	bIsDead = true;

	CameraComp->SetActive(false);
	DeathCameraComp->SetActive(true);

	// 3p mesh is now visible to local player
	DefaultMesh->SetVisibility(false);
	Mesh3P->SetOwnerNoSee(false);

	// Disables all actions
	UpdateMovementSettings(EMovementState::IDLE);

	// temp 
	HUD->HUDWidget->RemoveFromParent();

	APlayerController* PlayerController = Cast<APlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if(PlayerController)
	{
		PlayerController->SetIgnoreMoveInput(true);
		bCanCrouch = false;
	}

	bUseControllerRotationYaw = false;

	if(HasAuthority())
	{
		Multi_PlayMontage(Mesh3P, DeathMontageArray[UKismetMathLibrary::RandomIntegerInRange(0, DeathMontageArray.Num() - 1)]);
	}

	else
	{
		Server_PlayMontage(Mesh3P, DeathMontageArray[UKismetMathLibrary::RandomIntegerInRange(0, DeathMontageArray.Num() - 1)]);

		// Control rotation not updating from client to server without this
		Server_Death(false);
	}

	GetWorldTimerManager().SetTimer(RespawnHandle, this, &AC_PlayerCharacter::Respawn, 3.0f, false);

	EquippedWeaponArray[0]->SetActorHiddenInGame(true);

	// ragdoll and replication
	// remove capsule collision

	//GetWorldTimerManager().ClearAllTimersForObject(this);

}

void AC_PlayerCharacter::Respawn()
{
	// physics interaction will need to spawn a physics actor, as mesh must be the root component
	Server_Ragdoll(GetActorTransform(), this);

	// spawns new player in gamemode
	//RespawnPlayer.Broadcast();
}

void AC_PlayerCharacter::Server_Death_Implementation(bool bDead)
{
	bUseControllerRotationYaw = bDead;
}

void AC_PlayerCharacter::Server_Ragdoll_Implementation(FTransform RagdollSpawnTransform, AC_PlayerCharacter* PlayerToHide)
{
	FActorSpawnParameters SpawnParams;
	GetWorld()->SpawnActor<AActor>(RagdollPlayerClass, RagdollSpawnTransform.GetLocation(), RagdollSpawnTransform.GetRotation().Rotator(), SpawnParams);
	PlayerToHide->SetActorHiddenInGame(true);
}





# pragma endregion


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

	PlayerInputComponent->BindAction("Melee", IE_Pressed, this, &AC_PlayerCharacter::MeleeTracking);
}