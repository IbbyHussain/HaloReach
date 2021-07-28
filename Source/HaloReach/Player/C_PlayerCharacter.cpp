
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

#include "HaloReach/Libraries/C_SpawnLibrary.h"

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
	bIsMeleeAttacking = false;

	bCanMelee = true;

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

	if (FMeleeTrackCurve)
	{
		// Now we set the functions and some values.
		MeleeTrackTimeline->AddInterpFloat(FMeleeTrackCurve, MeleeTrackInterpFunction, FName("Bravo"));
		MeleeTrackTimeline->SetTimelineFinishedFunc(MeleeTrackTimelineFinished);
		MeleeTrackTimeline->SetLooping(false);
	}

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AC_PlayerCharacter::OnOverlapBegin);

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
		AC_BaseGun* Gun2 = Cast<AC_BaseGun>(EquippedWeaponArray[1]);

		HUD->UpdateWeaponReserves();

		if(Gun)
		{
			Gun2->StopReturnTimeline();

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

void AC_PlayerCharacter::OnRep_Switch()
{
	AC_BaseGun* Gun = Cast<AC_BaseGun>(EquippedWeaponArray[0]);
	Mesh3P->GetAnimInstance()->Montage_Play(Gun->GetWeapon3PEquipMontage(), 1.0f);
}

void AC_PlayerCharacter::Server_Switch_Implementation(UAnimMontage* Montage)
{
	Mesh3P->GetAnimInstance()->Montage_Play(Montage, 1.0f);
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
				// The OnRep only works on server 
				bIsReloading = !bIsReloading;
				
				// Call Server RPC for clients 
				if (!HasAuthority())
				{
					Server_Reload(Gun->GetWeapon3PReloadMontage(), Gun);
					Server_GunReload(Gun);
				}

				GunReload(Gun);

				DefaultMesh->GetAnimInstance()->Montage_Play(Gun->GetWeaponReloadMontage(), 1.0f);
				
				bCanReload = false;
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

void AC_PlayerCharacter::OnRep_Reload()
{
	AC_BaseGun* Gun = Cast<AC_BaseGun>(EquippedWeaponArray[0]);
	Mesh3P->GetAnimInstance()->Montage_Play(Gun->GetWeapon3PReloadMontage(), 1.0f);
}

void AC_PlayerCharacter::Server_Reload_Implementation(UAnimMontage* Montage, AC_BaseGun* BaseGun)
{
	Mesh3P->GetAnimInstance()->Montage_Play(Montage, 1.0f);
}

void AC_PlayerCharacter::ResetCanReload()
{
	bCanReload = true;
	bCanSwitch = true;
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

// Melee attack

void AC_PlayerCharacter::StartMelee()
{
	if(bCanMelee)
	{

		ClearActorsIgnoredArray();

		bIsMeleeAttacking = !bIsMeleeAttacking;

		EndFire();

		AC_BaseWeapon* Weapon = EquippedWeaponArray[0];

		if(Weapon)
		{
			PlayMontage(Weapon->GetWeapon1PMeleeMontage()); // 1p

			if(!HasAuthority())
			{
				Server_Melee(Weapon->GetWeapon3PMeleeMontage());
			}

			bCanMelee = false;
			bCanZoom = false;
			bCanSwitch = false;
			bCanFire = false;
			bCanReload = false;

			GetWorldTimerManager().SetTimer(MeleeHandle, this, &AC_PlayerCharacter::ResetMelee, Weapon->MeleeTime, false);
		}

		EndZoom();
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

void AC_PlayerCharacter::OnRep_Melee()
{
	AC_BaseWeapon* Weapon = EquippedWeaponArray[0];
	Mesh3P->GetAnimInstance()->Montage_Play(Weapon->GetWeapon3PMeleeMontage(), 1.0f);
}

void AC_PlayerCharacter::Server_Melee_Implementation(UAnimMontage* Montage)
{
	Mesh3P->GetAnimInstance()->Montage_Play(Montage, 1.0f);
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

void AC_PlayerCharacter::MeleeAttack(USkeletalMeshComponent* MeshComp, float Damage)
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
			Server_MeleeAttack(HitPlayer, Damage);
		}

		UGameplayStatics::ApplyDamage(HitPlayer, Damage, UGameplayStatics::GetPlayerController(this, 0), this, NULL);

		// Stops player from being damaged multiple times
		ActorsIgnored.Emplace(HitPlayer);
	}
}

void AC_PlayerCharacter::Server_MeleeAttack_Implementation(AActor* HitActor, float Damage)
{
	UGameplayStatics::ApplyDamage(HitActor, Damage, UGameplayStatics::GetPlayerController(this, 0), this, NULL);
}

// called on input 
void AC_PlayerCharacter::MeleeTracking()
{
	EnemyMap.Empty();

	FVector StartLocation = DefaultMesh->GetSocketLocation(MeleeStartSocket);
	FVector EndLocation = (GetActorRotation().Vector().ForwardVector * 0.0f) + StartLocation;
	FVector HalfSize = FVector(75.0f, 25.0f, 200.0f);
	FRotator Orientation = GetActorRotation();

	ETraceTypeQuery BoxTrace = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility);


	TArray<FHitResult> Hits;

	bool bHit = UKismetSystemLibrary::BoxTraceMulti(GetWorld(), StartLocation, EndLocation, HalfSize, Orientation, BoxTrace, false, IgnoredActorsTracking, EDrawDebugTrace::ForDuration, Hits, true, FLinearColor::Blue, FLinearColor::Black);

	//TArray<float>DistancesToPlayer;

	for (auto x : Hits)
	{
		AC_PlayerCharacter* Enemy = Cast<AC_PlayerCharacter>(x.GetActor());

		if(Enemy)
		{
			FVector PlayerLocation = GetActorLocation();
			FVector EnemyLocation = Enemy->GetActorLocation();

			float Distance = (EnemyLocation - PlayerLocation).Size();

			//DistancesToPlayer.Emplace(Distance);

			EnemyMap.Emplace(Distance, Enemy);
		}
	}

	if (EnemyMap.Num() > 0) //DistancesToPlayer.Num() > 0
	{
		TArray<float> DistancePlayer;
		EnemyMap.GenerateKeyArray(DistancePlayer);


		// Finds the smallest value in the array, in this case the shortest distance
		ShortestDistance = DistancePlayer[0]; //DistancesToPlayer[0]
		for (int i = 0; i < DistancePlayer.Num(); i++) //DistancesToPlayer.Num()
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Value: %f"), DistancePlayer[i])); //DistancesToPlayer[i]

			if (DistancePlayer[i] < ShortestDistance) //DistancesToPlayer[i]
			{
				ShortestDistance = DistancePlayer[i]; //DistancesToPlayer[i]
			}

		}

		if (ShortestDistance > 150.0f) // 150 is melee range for nomral melee to hit
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
}

void AC_PlayerCharacter::MeleeTrackTimelineFloatReturn(float Value)
{
	FVector PlayerLocation = GetActorLocation();

	LocX = PlayerLocation.X;
	LocY = PlayerLocation.Y;
	LocZ = PlayerLocation.Z;

	AC_PlayerCharacter* ClosestEnemy;
	ClosestEnemy = *(EnemyMap.Find(ShortestDistance));

	FVector EnemyLocation = ClosestEnemy->GetActorLocation();

	SetActorLocation(FMath::Lerp(GetActorLocation(), EnemyLocation, Value));

	UKismetMathLibrary::FindLookAtRotation(PlayerLocation, EnemyLocation);
	

	if(bIsOverlappingEnemy(ClosestEnemy))
	{
		StopMeleeTrackTimeline();
		StartMelee();
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, FString::Printf(TEXT("COLLIDED")));
	}

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC)
	{
		//UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), EnemyLocation)
		//PC->SetControlRotation(FMath::Lerp(GetActorRotation(), UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), EnemyLocation), Value));
		PC->SetControlRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), EnemyLocation));
	}
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

void AC_PlayerCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

void AC_PlayerCharacter::StopMeleeTrackTimeline()
{
	if(MeleeTrackTimeline->IsPlaying())
	{
		MeleeTrackTimeline->Stop();
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
	DOREPLIFETIME(AC_PlayerCharacter, bIsMeleeAttacking);
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

void AC_PlayerCharacter::SetControlRotation()
{
	if(HasAuthority() || IsLocallyControlled())
	{
		ControlRotation = GetController()->GetControlRotation();
	}
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

	PlayerInputComponent->BindAction("Melee", IE_Pressed, this, &AC_PlayerCharacter::MeleeTracking);
}