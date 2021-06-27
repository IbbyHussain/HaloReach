
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
#include "Net/UnrealNetwork.h"

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

	CurrentWeapon = nullptr;
	HolsteredWeapon = nullptr;
}

void AC_PlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	CameraManager = Cast<AC_PlayerCameraManager>(UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0));

	HUD = Cast<AC_PlayerHUD>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD());
	//HUD = Cast<AC_PlayerHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());

	// Spawns the equipped and holstered weapon in third person
	SpawnWeapon3P();

	// Spawns default gun, used in first person -- important to do combat stuff on server
	if(HasAuthority())
	{
		//// Spawns the equipped weapon in third person
		//SpawnWeapon3P(Combat.Weapon3P, Combat.WeaponClass3P, ("ARSocket"));

		//// Spawns the holstered weapon in third person
		//SpawnWeapon3P(Combat.HolsteredWeapon3P, Combat.HolsteredWeaponClass3P, ("Rifle_3P_Holstered_Socket"));

		SetupWeaponSwitching(DefaultWeaponClass, ("AR_1P_Socket"), ("AR_3P_Socket"), true);
	}



	//Update3PWeapons(("AR_3P_Socket"));

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
void AC_PlayerCharacter::SpawnWeapon(TSubclassOf<AC_BaseWeapon> WeaponClass, FName WeaponSocket)
{
	//Spawn new weapon in first person
	FActorSpawnParameters SpawnParams;

	FTransform WeaponTransform = DefaultMesh->GetSocketTransform(WeaponSocket, ERelativeTransformSpace::RTS_World);
	FVector WeaponSpawnLocation = WeaponTransform.GetLocation();
	FRotator WeaponSpawnRotation = WeaponTransform.GetRotation().Rotator();

	CurrentWeapon = GetWorld()->SpawnActor<AC_BaseWeapon>(WeaponClass, WeaponSpawnLocation, WeaponSpawnRotation, SpawnParams); //Combat.CurrentWeapon
	CurrentWeapon->AttachToComponent(DefaultMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, WeaponSocket);
	CurrentWeapon->SetOwner(this);
}

// Handles switching of weapons
void AC_PlayerCharacter::SetupWeaponSwitching(TSubclassOf<AC_BaseWeapon> WeaponClass, FName WeaponSocket, FName Weapon3PSocket, bool bUpdateCurrentWeapon)
{
	// Sets the holstered weapon to the current weapon, then destroys current weapon
	if(bUpdateCurrentWeapon)
	{
		if (CurrentWeapon)
		{
			HolsteredWeapon = CurrentWeapon;
			CurrentWeapon->Destroy();
		}
	}

	SpawnWeapon(WeaponClass, WeaponSocket);

	DefaultMesh->GetAnimInstance()->Montage_Play(CurrentWeapon->GetWeaponEquipMontage(), 1.0f);

	// Set the weapon type
	WeaponType = CurrentWeapon->Type;
	OnWeaponTypeUpdate();

	UpdateWeapon3P(Combat.Weapon3P, CurrentWeapon, Weapon3PSocket);

	// Update the 3p holstered weapon mesh
	if (HolsteredWeapon)
	{
		UpdateWeapon3P(Combat.HolsteredWeapon3P, HolsteredWeapon, HolsteredWeapon->Socket3PHolstered);
	}
	//UpdateWeapon3P(Weapon3PSocket);
}

void AC_PlayerCharacter::SwitchWeapons()
{
	if(HasAuthority())
	{
		Multi_SwitchWeapons();
	}

	else
	{
		Server_SwitchWeapons();
	}
}

void AC_PlayerCharacter::Server_SwitchWeapons_Implementation()
{
	Multi_SwitchWeapons();
}

void AC_PlayerCharacter::Multi_SwitchWeapons_Implementation()
{
	if (HolsteredWeapon)
	{
		SetupWeaponSwitching(HolsteredWeapon->BPRef, HolsteredWeapon->Socket1P, HolsteredWeapon->Socket3P, true);
	}
}

// When we pick up weapon
void AC_PlayerCharacter::UpdateWeapons()
{
	// If there is no holstered weapon, make current weapon holstered and equip new weapon
	if (CurrentWeapon && (HolsteredWeapon == nullptr))
	{
		HolsteredWeapon = CurrentWeapon;
		UE_LOG(LogTemp, Log, TEXT("Swap 1"));
		CurrentWeapon->Destroy();
	}

	// If there is a holstered and current weapon, swap current weapon for new weapon. This keeps holstered weapon
	else if(CurrentWeapon && HolsteredWeapon)
	{
		CurrentWeapon = CurrentWeapon;
		UE_LOG(LogTemp, Log, TEXT("Swap 2"));
		CurrentWeapon->Destroy();
	}
}

void AC_PlayerCharacter::SpawnWeapon3P()
{
	if(HasAuthority())
	{
		// Spawn a currently equipped weapon in 3P 
		FActorSpawnParameters SpawnParams;

		FTransform Weapon3PTransform = Mesh3P->GetSocketTransform(("ARSocket"), ERelativeTransformSpace::RTS_World);
		FVector WeaponSpawn3PLocation = Weapon3PTransform.GetLocation();
		FRotator WeaponSpawn3PRotation = Weapon3PTransform.GetRotation().Rotator();

		Combat.Weapon3P = GetWorld()->SpawnActor<AC_Weapon3P>(Combat.WeaponClass3P, WeaponSpawn3PLocation, WeaponSpawn3PRotation, SpawnParams);
		Combat.Weapon3P->SetOwner(this);

		// Spawn a holstered weapon in 3P
		FTransform Weapon3PHolsteredTransform = Mesh3P->GetSocketTransform(("Rifle_3P_Holstered_Socket"), ERelativeTransformSpace::RTS_World);
		FVector WeaponSpawn3PHolsteredLocation = Weapon3PHolsteredTransform.GetLocation();
		FRotator WeaponSpawn3PHolsteredRotation = Weapon3PHolsteredTransform.GetRotation().Rotator();

		Combat.HolsteredWeapon3P = GetWorld()->SpawnActor<AC_Weapon3P>(Combat.HolsteredWeaponClass3P, WeaponSpawn3PHolsteredLocation, WeaponSpawn3PHolsteredRotation, SpawnParams);
		Combat.HolsteredWeapon3P->SetOwner(this);
	}

	else
	{
		Server_SpawnWeapon3P();
	}
	
}

void AC_PlayerCharacter::Server_SpawnWeapon3P_Implementation()
{
	// Spawn a currently equipped weapon in 3P 
	FActorSpawnParameters SpawnParams;

	FTransform Weapon3PTransform = Mesh3P->GetSocketTransform(("ARSocket"), ERelativeTransformSpace::RTS_World);
	FVector WeaponSpawn3PLocation = Weapon3PTransform.GetLocation();
	FRotator WeaponSpawn3PRotation = Weapon3PTransform.GetRotation().Rotator();

	Combat.Weapon3P = GetWorld()->SpawnActor<AC_Weapon3P>(Combat.WeaponClass3P, WeaponSpawn3PLocation, WeaponSpawn3PRotation, SpawnParams);
	Combat.Weapon3P->SetOwner(this);

	// Spawn a holstered weapon in 3P
	FTransform Weapon3PHolsteredTransform = Mesh3P->GetSocketTransform(("Rifle_3P_Holstered_Socket"), ERelativeTransformSpace::RTS_World);
	FVector WeaponSpawn3PHolsteredLocation = Weapon3PHolsteredTransform.GetLocation();
	FRotator WeaponSpawn3PHolsteredRotation = Weapon3PHolsteredTransform.GetRotation().Rotator();

	Combat.HolsteredWeapon3P = GetWorld()->SpawnActor<AC_Weapon3P>(Combat.HolsteredWeaponClass3P, WeaponSpawn3PHolsteredLocation, WeaponSpawn3PHolsteredRotation, SpawnParams);
	Combat.HolsteredWeapon3P->SetOwner(this);
}

void AC_PlayerCharacter::UpdateWeapon3P(AC_Weapon3P*& Weapon, AC_BaseWeapon* NewWeapon, FName Weapon3PSocket)
{
	// Attach new 3P Weapon mesh to 3P character mesh
	Weapon->AttachToComponent(Mesh3P, FAttachmentTransformRules::SnapToTargetIncludingScale, Weapon3PSocket); //Combat.Weapon3P

	// Set the skeletal mesh of 3P gun to the 1P gun
	Weapon->WeaponMesh3P->SetSkeletalMesh(NewWeapon->WeaponMesh->SkeletalMesh); //Combat.Weapon3P
}

//void AC_PlayerCharacter::UpdateWeapon3P(FName Weapon3PSocket)
//{
//	// Updates the current 3P weapon
//	Combat.Weapon3P->AttachToComponent(Mesh3P, FAttachmentTransformRules::SnapToTargetIncludingScale, Weapon3PSocket);
//	Combat.Weapon3P->WeaponMesh3P->SetSkeletalMesh(CurrentWeapon->WeaponMesh->SkeletalMesh);
//
//	if(HolsteredWeapon)
//	{
//		Combat.HolsteredWeapon3P->AttachToComponent(Mesh3P, FAttachmentTransformRules::SnapToTargetIncludingScale, HolsteredWeapon->Socket3PHolstered);
//		Combat.HolsteredWeapon3P->WeaponMesh3P->SetSkeletalMesh(HolsteredWeapon->WeaponMesh->SkeletalMesh);
//	}
//}

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

	DOREPLIFETIME(AC_PlayerCharacter, CurrentWeapon);
	DOREPLIFETIME(AC_PlayerCharacter, HolsteredWeapon);
	//DOREPLIFETIME(AC_PlayerCharacter, Combat);

}

void AC_PlayerCharacter::Update3PWeapons(FName Weapon3PSocket)
{
	//if (HasAuthority()) 
	//{
	//	UpdateWeapon3P(Combat.Weapon3P, CurrentWeapon, Weapon3PSocket);

	//	// Update the 3p holstered weapon mesh
	//	if (HolsteredWeapon)
	//	{
	//		UpdateWeapon3P(Combat.HolsteredWeapon3P, HolsteredWeapon, HolsteredWeapon->Socket3PHolstered);
	//	}
	//}

	//else
	//{
	//	Server_Update3PWeapons(Weapon3PSocket);
	//}
}

void AC_PlayerCharacter::Server_Update3PWeapons_Implementation(FName Weapon3PSocket)
{
	Multi_Update3PWeapons(Weapon3PSocket);
}

void AC_PlayerCharacter::Multi_Update3PWeapons_Implementation(FName Weapon3PSocket)
{
	// Update the 3P weapon mesh
	//UpdateWeapon3P(Weapon3PSocket);
}





///////////////////////////////////////

// organise 

FVector AC_PlayerCharacter::GetPawnViewLocation() const
{
	return CameraComp->GetComponentLocation();
}

void AC_PlayerCharacter::StartFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->Attack();
		//UE_LOG(LogTemp, Log, TEXT("Player: Attacked!"));
	}
}

void AC_PlayerCharacter::EndFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StopAttack();
		//UE_LOG(LogTemp, Log, TEXT("Player: STOP Attacked!"));
	}
}

void AC_PlayerCharacter::BeginZoom()
{
	bZoomIn = true;
}

void AC_PlayerCharacter::EndZoom()
{
	bZoomIn = false;
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

	//PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AC_PlayerCharacter::FireWeapon);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AC_PlayerCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AC_PlayerCharacter::EndFire);

	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &AC_PlayerCharacter::BeginZoom);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &AC_PlayerCharacter::EndZoom);
}