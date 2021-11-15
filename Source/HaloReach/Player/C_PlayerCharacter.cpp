
#include "C_PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerExtra/C_PlayerCameraManager.h"
#include "Components/BoxComponent.h"
#include "HaloReach/Components/C_HealthComponent.h"
#include "HaloReach/UI/HUD/C_PlayerHUD.h"
#include "DrawDebugHelpers.h"
#include "HaloReach/Interactables/C_BasePickup.h"
#include "HaloReach/Player/PlayerExtra/C_ReachPlayerController.h"
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
#include "HaloReach/Components/C_RadarIconComponent.h"

#include "HaloReach/Libraries/C_SpawnLibrary.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "HaloReach/Player/PlayerExtra/C_ReachPlayerState.h"
#include "Components/WidgetComponent.h"
#include "HaloReach/UI/C_PlayerNameWidget.h"
#include "HaloReach/Interactables/Weapons/C_BaseGrenade.h"

#include "HaloReach/GameModes/C_ReachGameStateBase.h"
#include "Animation/AnimMontage.h"
#include "HaloReach/Interfaces/C_BaseSaveGame.h"

//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("texthere: %f"), x));

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

	RadarComp = CreateDefaultSubobject<UC_RadarIconComponent>(TEXT("RadarComponent"));

	//Crouch Timeline
	CrouchTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("CrouchTimeline"));
	CrouchInterpFunction.BindUFunction(this, FName("CrouchTimelineFloatReturn"));

	// Death Camera components

	DeathSpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("Death SpringArm"));
	DeathSpringArmComp->SetupAttachment(RootComponent);
	DeathSpringArmComp->TargetArmLength = 500.0f;

	DeathCameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Death Camera"));
	DeathCameraComp->SetupAttachment(DeathSpringArmComp, USpringArmComponent::SocketName);

	PlayerNameWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("Player Name Widget Comp"));
	PlayerNameWidgetComp->SetupAttachment(RootComponent);

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
	bCanThrowGrenade = true;

	bCanMelee = true;

	DefaultSpeed = 500.0f;
	CrouchSpeed = 150.0f;
	
	ActorsIgnored = { this };
	IgnoredActorsTracking = { this };

	MeleeTrackTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("Melee Track Timeline"));

	MeleeTrackInterpFunction.BindUFunction(this, FName("MeleeTrackTimelineFloatReturn"));
	MeleeTrackTimelineFinished.BindUFunction(this, FName("OnMeleeTrackTimelineFinished"));

	NameWidgetTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("NameWidgetTimeline"));
	NameWidgetInterpFunction.BindUFunction(this, FName("NameWidgetTimelineFloatReturn"));

	PlayerNameBoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Comp"));
	PlayerNameBoxComp->SetupAttachment(RootComponent);

	MontageEndedDelegate.BindUObject(this, &AC_PlayerCharacter::OnGrenadeStartMontageFinished);

	ThrowForce = 2000.0f;

}

void AC_PlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// General pointer definitions 

	CameraManager = Cast<AC_PlayerCameraManager>(UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0));

	HUD = Cast<AC_PlayerHUD>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD());

	PlayerNameWidget = Cast<UC_PlayerNameWidget>(PlayerNameWidgetComp->GetUserWidgetObject());

	PC = Cast<APlayerController>(GetController());

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

	//if (HasAuthority())
	//{
	//	// respawn player at a player start 
	//	RespawnPlayer.Broadcast(this);
	//}

	//else
	//{
	//	Server_Broadcast(this);
	//}

	PlayerNameBoxComp->OnComponentBeginOverlap.AddDynamic(this, &AC_PlayerCharacter::OnOverlapBegin);
	PlayerNameBoxComp->OnComponentEndOverlap.AddDynamic(this, &AC_PlayerCharacter::OnOverlapEnd);

	if (FNameWidgetCurve)
	{
		// Now we set the functions and some values.
		NameWidgetTimeline->AddInterpFloat(FNameWidgetCurve, NameWidgetInterpFunction, FName("Charlie"));
		NameWidgetTimeline->SetLooping(false);
	}

	PlayerNameWidgetComp->GetUserWidgetObject()->SetRenderScale(FVector2D(1.0f, 1.0f));

	// Tests

	Grenades.GrenadesArray.Emplace(Grenades.FragGrenadeClass);
	Grenades.GrenadesArray.Emplace(Grenades.PlasmaGrenadeClass);
	Grenades.EquippedGrenadeClass = Grenades.GrenadesArray[0];
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

void AC_PlayerCharacter::OnHealthChanged(UC_HealthComponent* HealthComponent, float Health, bool bUpdateCombatState, AActor* PlayerKiller)
{
	HUD->HUDUpdateHealthImage(HealthComp->GetHealth());

	if(bUpdateCombatState)
	{
		// bIsDead so that death() is only called once
		if (HealthComp->GetHealth() <= 0.0f && !bIsDead)
		{
			Death(PlayerKiller);
			BPDeath(PlayerKiller);
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

void AC_PlayerCharacter::LookUp(float Axis)
{
	AddControllerPitchInput(Axis);
}

void AC_PlayerCharacter::LookRight(float Axis)
{
	AddControllerYawInput(Axis);
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

	case EMovementState::NOTIDLE:
		bCanJump = true;
		bCanCrouch = true;
		bCanFire = true;
		bCanReload = true;
		bCanZoom = true;
		bCanMelee = true;
		bCanSwitch = true;

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

# pragma region Interact

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

# pragma endregion 

// COMBAT SYSTEM 

# pragma region  Weapon Switching

void AC_PlayerCharacter::SwitchWeapons()
{
	if(bCanSwitch)
	{
		UpdateRestrictionState(ERestrictionState::RESTRICTED);

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
			

			

			UE_LOG(LogTemp, Log, TEXT("Weapon switch length is: %f"), Gun->WeaponStats.SwitchLength);

			GetWorldTimerManager().SetTimer(SwitchResetHandle, this, &AC_PlayerCharacter::ResetCanSwitch, Gun->WeaponStats.SwitchLength, false);
		}
	}
}

void AC_PlayerCharacter::ResetCanSwitch()
{
	UpdateRestrictionState(ERestrictionState::FREE);
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
				UpdateRestrictionState(ERestrictionState::RESTRICTED);

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
	UpdateRestrictionState(ERestrictionState::FREE);
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

void AC_PlayerCharacter::UpdateRestrictionState(ERestrictionState NewState)
{
	switch(NewState)
	{
	case ERestrictionState::RESTRICTED:

		EndFire();
		EndZoom();

		// Very restrictive - cant do any actions while these happen
		bCanMelee = false;
		bCanSwitch = false;
		bCanReload = false;

		// can be interrupted
		bCanZoom = false;
		bCanFire = false;
		bCanThrowGrenade = false;
		break;

	case ERestrictionState::FREE:

		bCanMelee = true;
		bCanSwitch = true;
		bCanReload = true;

		bCanZoom = true;
		bCanFire = true;
		bCanThrowGrenade = true;
		break;

	}
}

# pragma region Melee Attack

// called on input binding
void AC_PlayerCharacter::MeleeTracking()
{
	if (bCanMelee)
	{
		// Restrictions 
		UpdateRestrictionState(ERestrictionState::RESTRICTED);

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

	//EnemyLocation = ClosestEnemy->GetActorLocation();

	EnemyLocation = ClosestEnemy->DefaultMesh->GetComponentLocation();

	if (!HasAuthority())
	{
		Server_StartMelee(PlayerLoc, EnemyLocation, Value);
	}

	SetActorLocation(FMath::Lerp(PlayerLoc, EnemyLocation, Value));

	UKismetMathLibrary::FindLookAtRotation(PlayerLoc, EnemyLocation);


	if (bIsOverlappingEnemy(ClosestEnemy->GetCapsuleComponent()))
	{
		StopMeleeTrackTimeline();
		StartMelee();
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, FString::Printf(TEXT("COLLIDED")));
	}

	if (PC)
	{
		PC->SetControlRotation(FMath::Lerp(GetActorRotation(), UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), EnemyLocation), Value));
		//PC->SetControlRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), EnemyLocation));
	}
}

void AC_PlayerCharacter::OnMeleeTrackTimelineFinished()
{
	StartMelee();
}

void AC_PlayerCharacter::StopMeleeTrackTimeline()
{
	if (MeleeTrackTimeline->IsPlaying())
	{
		MeleeTrackTimeline->Stop();
	}
}

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
	UpdateRestrictionState(ERestrictionState::FREE);

}

void AC_PlayerCharacter::ClearActorsIgnoredArray()
{
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

// Set Actor location is server only
void AC_PlayerCharacter::Server_StartMelee_Implementation(FVector StartLocation, FVector EndLocation, float Alpha)
{
	SetActorLocation(FMath::Lerp(StartLocation, EndLocation, Alpha));
}

bool AC_PlayerCharacter::bIsOverlappingEnemy(UCapsuleComponent* Enemy)
{
	if (GetCapsuleComponent()->IsOverlappingComponent(Enemy))
	{
		return true;
	}

	else
	{
		return false;
	}
}

# pragma endregion

void AC_PlayerCharacter::Server_SetBool_Implementation(bool bNew)
{
	bIsHoldingGrenade = bNew;
}

# pragma region Grenade

// when grenade input is held or pressed
void AC_PlayerCharacter::ThrowGrenade()
{
	if(Grenades.EquippedGrenadeAmount > 0 && bCanThrowGrenade)
	{
		UpdateRestrictionState(ERestrictionState::RESTRICTED);

		bCanSwitchGrenades = false;

		if (HasAuthority())
		{
			bIsHoldingGrenade = true;
		}

		else
		{
			Server_SetBool(true);
		}

		bIsHoldingGrenade = true;

		Server_SpawnGrenade(this, GrenadeSocket, DefaultMesh, Grenades.EquippedGrenadeClass);

		if (HasAuthority())
		{
			EquippedGrenade->AttachToComponent(DefaultMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, GrenadeSocket);
			Multi_PlayMontage(Mesh3P, GrenadeThrowStartMontage);
			Mesh3P->GetAnimInstance()->Montage_SetEndDelegate(MontageEndedDelegate);
		}
	

		else
		{
			Server_AttachGrenade(GrenadeSocket);
			Server_PlayMontage(Mesh3P, GrenadeThrowStartMontage);
			Server_BindMontageDelegate(Mesh3P);
		}
	}
}

void AC_PlayerCharacter::Server_BindMontageDelegate_Implementation(USkeletalMeshComponent* MeshComp)
{
	MeshComp->GetAnimInstance()->Montage_SetEndDelegate(MontageEndedDelegate);
}

void AC_PlayerCharacter::Server_AttachGrenade_Implementation(FName b)
{
	EquippedGrenade->AttachToComponent(DefaultMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, b);
}

// when grenade input is released
void AC_PlayerCharacter::ReleaseGrenade()
{
	if(Grenades.EquippedGrenadeAmount > 0 && bIsHoldingGrenade)
	{
		if (HasAuthority())
		{
			bIsHoldingGrenade = false;
		}

		else
		{
			Server_SetBool(false);
		}

		bIsHoldingGrenade = false;

		if (HasAuthority())
		{
			Multi_PlayMontage(Mesh3P, GrenadeThrowReleaseMontage);
		}

		else
		{
			Server_PlayMontage(Mesh3P, GrenadeThrowReleaseMontage);
		}

		Mesh3P->GetAnimInstance()->Montage_Stop(0.1f, GrenadeThrowHoldMontage);

		UpdateGrenadeAmount(true);

		bCanSwitchGrenades = true;

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("texthere: %d"), Grenades.EquippedGrenadeAmount));
	}
}

void AC_PlayerCharacter::OnGrenadeStartMontageFinished(UAnimMontage* Montage, bool bInterrupted)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Delegate binded success")));

	if(bIsHoldingGrenade)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Delegate binded success")));
		if (HasAuthority())
		{
			Multi_PlayMontage(Mesh3P, GrenadeThrowHoldMontage);
		}

		else
		{
			Server_PlayMontage(Mesh3P, GrenadeThrowHoldMontage);

		}
	}
}

void AC_PlayerCharacter::Server_SpawnGrenade_Implementation(AC_PlayerCharacter* PlayerOwner, FName b, USkeletalMeshComponent* c, TSubclassOf<AC_BaseGrenade> GrenadeToSpawn)
{
	EquippedGrenade = UC_SpawnLibrary::SpawnActorAtLocation(GetWorld(), GrenadeToSpawn, EquippedGrenade, GetActorLocation(), GetActorRotation());
	EquippedGrenade->SetOwner(PlayerOwner);
}

void AC_PlayerCharacter::Server_PrepGrenade_Implementation(FVector ImpulseDirection)
{
	if (EquippedGrenade)
	{
		EquippedGrenade->DetachFromActor(FDetachmentTransformRules(EDetachmentRule::KeepWorld, false));
		EquippedGrenade->Thrown(ImpulseDirection);
		EquippedGrenade->Multi_bSetOnlyOwnerSeeMesh(false);
		EquippedGrenade = nullptr;
	}
}

// used in anim notify to launch grenade from players hand
void AC_PlayerCharacter::Client_LaunchGrenade_Implementation()
{
	FVector CameraUpVector = CameraComp->GetUpVector();
	FVector CameraForwardVector = CameraComp->GetForwardVector();

	FVector LaunchForce = (FMath::Lerp(CameraUpVector, CameraForwardVector, 1.0f)) * ThrowForce;

	Server_PrepGrenade(LaunchForce);
}

void AC_PlayerCharacter::SwitchGrenades()
{
	if(bCanSwitchGrenades)
	{
		// Will switch grenade, assumes frag grenade is the starting grenade
		Grenades.GrenadesArray.Swap(0, 1);
		Grenades.EquippedGrenadeClass = Grenades.GrenadesArray[0];

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Current grenade class: %s"), *Grenades.EquippedGrenadeClass->GetName()));

		UpdateGrenadeAmount(false);
	}
}

void AC_PlayerCharacter::UpdateGrenadeAmount(bool bDecrementGrenadeAmount)
{
	// Will set the equipped grenade Amount to correct amount depending on what grenade is equipped
	if(Grenades.EquippedGrenadeClass == Grenades.FragGrenadeClass)
	{
		// Will decrement amount when a grenade is released.
		if(bDecrementGrenadeAmount)
		{
			Grenades.FragGrenadeAmount--;
		}

		Grenades.EquippedGrenadeAmount = Grenades.FragGrenadeAmount;
		Grenades.SecondaryGrenadeAmount = Grenades.PlasmaGrenadeAmount;

	}

	else
	{
		if (bDecrementGrenadeAmount)
		{
			Grenades.PlasmaGrenadeAmount--;
		}

		Grenades.EquippedGrenadeAmount = Grenades.PlasmaGrenadeAmount;
		Grenades.SecondaryGrenadeAmount = Grenades.FragGrenadeAmount;
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("FragGrenades: %d, PlasmaGrenades: %d, EquippedGrenades: %d"), 
		Grenades.FragGrenadeAmount, Grenades.PlasmaGrenadeAmount, Grenades.EquippedGrenadeAmount));
}

	 

# pragma endregion

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
	if((HasAuthority() || IsLocallyControlled()) && GetController())
	{
		ControlRotation = GetController()->GetControlRotation();
	}
}

# pragma region Player Name

void AC_PlayerCharacter::Server_SetPlayerName_Implementation(const FString& NewPlayerName)
{
	Multi_SetPlayerName(NewPlayerName);
}

void AC_PlayerCharacter::Multi_SetPlayerName_Implementation(const FString& NewPlayerName)
{
	if(PlayerNameWidget)
	{
		PlayerNameWidget->DisplayedPlayerName = NewPlayerName;

		// Does not need to be called everytime
		AC_ReachPlayerController* RPC = Cast<AC_ReachPlayerController>(PC);
		if(RPC)
		{
			RPC->AssignedName = NewPlayerName;
		}
	}
}

void AC_PlayerCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Makes the render scale for the name widget 3x
	AC_PlayerCharacter* PlayerCharacter = Cast<AC_PlayerCharacter>(OtherActor);
	if(PlayerCharacter && OtherActor != this)
	{
		if(PlayerCharacter->IsLocallyControlled())
		{
			NameWidgetScaleX = 3.0f;
			NameWidgetScaleY = 3.0f;

			NameWidgetTimeline->PlayFromStart();
		}
	}
}

void AC_PlayerCharacter::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Makes the render scale for the name widget default
	AC_PlayerCharacter* PlayerCharacter = Cast<AC_PlayerCharacter>(OtherActor);
	if (PlayerCharacter && OtherActor != this)
	{
		if (PlayerCharacter->IsLocallyControlled())
		{
			NameWidgetScaleX = 1.0f;
			NameWidgetScaleY = 1.0f;

			NameWidgetTimeline->PlayFromStart();
		}
	}
}

void AC_PlayerCharacter::NameWidgetTimelineFloatReturn(float Value)
{
	if(PlayerNameWidgetComp->GetUserWidgetObject())
	{
		CurrentNameWidgetScaleX = PlayerNameWidgetComp->GetUserWidgetObject()->RenderTransform.Scale.X;
		CurrentNameWidgetScaleY = PlayerNameWidgetComp->GetUserWidgetObject()->RenderTransform.Scale.Y;

		// Lerps between current render scale and a new render scale
		PlayerNameWidgetComp->GetUserWidgetObject()->SetRenderScale(FVector2D(UKismetMathLibrary::Lerp(CurrentNameWidgetScaleX, NameWidgetScaleX, Value)));
		PlayerNameWidgetComp->GetUserWidgetObject()->SetRenderScale(FVector2D(UKismetMathLibrary::Lerp(CurrentNameWidgetScaleY, NameWidgetScaleY, Value)));
	}
}

# pragma endregion

# pragma region Player Killer Name

void AC_PlayerCharacter::Client_SetKillerName_Implementation(const FString& KillerActorName)
{
	// Set using Client RPC as Killer name exists locally
	PlayerKillerName = KillerActorName;
}

void AC_PlayerCharacter::Server_SetKillerName_Implementation(const FString& KillerActorName)
{
	Client_SetKillerName(KillerActorName);
}

void AC_PlayerCharacter::Client_CheckKillerName_Implementation(const FString& KillerActorName)
{
	// Checking Health needs to be done Client side
	if (HealthComp->GetHealth() <= HealthComp->MaxHealth)
	{
		Server_SetKillerName(KillerActorName);
	}
}

void AC_PlayerCharacter::Multi_UpdateAllPlayerDeathWidgets_Implementation(const FString& PlayerKiller, const FString& DeadPlayer)
{
	// Update all players 'Death Widgets' to show Player killer and the killed player
	AC_ReachGameStateBase* RGS = Cast<AC_ReachGameStateBase>(UGameplayStatics::GetGameState(GetWorld()));
	if (RGS)
	{
		RGS->UpdateGlobalDeaths(PlayerKiller, DeadPlayer);
	}
}

void AC_PlayerCharacter::Server_UpdateAllPlayerDeathWidgets_Implementation(const FString& PlayerKiller, const FString& DeadPlayer)
{
	Multi_UpdateAllPlayerDeathWidgets(PlayerKiller, DeadPlayer);
}

# pragma endregion

# pragma region Player Death

void AC_PlayerCharacter::Death(AActor* PlayerKiller)
{
	bIsDead = true;

	// Change to third person camera
	CameraComp->SetActive(false);
	DeathCameraComp->SetActive(true);

	// 3p mesh is now visible to local player
	DefaultMesh->SetVisibility(false);
	Mesh3P->SetOwnerNoSee(false);

	EquippedWeaponArray[0]->WeaponMesh->SetOwnerNoSee(true);

	// Disable player actions 
	UpdateMovementSettings(EMovementState::IDLE);

	// HUD Updates 
	HUD->HUDWidget->RemoveFromParent();
	HUD->CreateDeathWidget();

	// Stops player mesh from moving with camera, as player will move 3P camera
	bUseControllerRotationYaw = false;

	// Play random death montage and Update the global alert widget
	if (HasAuthority())
	{
		Multi_PlayMontage(Mesh3P, DeathMontageArray[UKismetMathLibrary::RandomIntegerInRange(0, DeathMontageArray.Num() - 1)]);

		Multi_UpdateAllPlayerDeathWidgets(PlayerKillerName, PlayerNameWidget->DisplayedPlayerName);
	}

	else
	{
		Server_PlayMontage(Mesh3P, DeathMontageArray[UKismetMathLibrary::RandomIntegerInRange(0, DeathMontageArray.Num() - 1)]);

		Server_UpdateAllPlayerDeathWidgets(PlayerKillerName, PlayerNameWidget->DisplayedPlayerName);

		// Control rotation not updating from client to server without this, OR collision
		Server_Death(false);
	}

	//GetWorldTimerManager().SetTimer(RagdollHandle, this, &AC_PlayerCharacter::StartRagdoll, 3.0f, false);

	GetWorldTimerManager().SetTimer(FadeOutEndHandle, this, &AC_PlayerCharacter::StartRespawn, 4.0f, false);
}

// Updates the Rotation Yaw settings for clients 
void AC_PlayerCharacter::Server_Death_Implementation(bool bDead)
{
	bUseControllerRotationYaw = bDead;
}

# pragma region Rgadoll

void AC_PlayerCharacter::StartRagdoll()
{
	Server_Ragdoll(Mesh3P->GetComponentTransform(), this);
	Server_DestroyWeapons();

	GetWorldTimerManager().ClearTimer(RagdollHandle);
}

void AC_PlayerCharacter::Server_Ragdoll_Implementation(FTransform RagdollSpawnTransform, AC_PlayerCharacter* PlayerToHide)
{
	FActorSpawnParameters SpawnParams;
	GetWorld()->SpawnActor<AActor>(RagdollPlayerClass, RagdollSpawnTransform.GetLocation(), RagdollSpawnTransform.GetRotation().Rotator(), SpawnParams);
	PlayerToHide->SetActorHiddenInGame(true);
}

# pragma endregion

# pragma region Respawn

void  AC_PlayerCharacter::StartRespawn()
{
	HUD->PlayHUDFadeInAnimation();
	Server_DestroyWeapons();
	SetActorHiddenInGame(true);

	GetWorldTimerManager().SetTimer(RespawnHandle, this, &AC_PlayerCharacter::Respawn, 1.0f, false);
}

void AC_PlayerCharacter::Respawn()
{
	// Broadcast to player controller to tell the gamemode to respawn the player
	//RespawnPlayer.Broadcast(this);

	//GetWorldTimerManager().ClearAllTimersForObject(this);

	if(HasAuthority())
	{
		RespawnPlayer.Broadcast(this);
	}

	else
	{
		AC_ReachPlayerController* RPC = Cast<AC_ReachPlayerController>(PC);
		if (RPC)
		{
			RPC->RespawnPlayer(this);

		}
	}

	//Destroy();

}

void AC_PlayerCharacter::Server_Broadcast_Implementation(AC_PlayerCharacter* Player)
{
	RespawnPlayer.Broadcast(Player);
}

void AC_PlayerCharacter::Server_DestroyWeapons_Implementation()
{
	// Weapons are spawned on server, so need to be destroyed there
	for (auto i : EquippedWeaponArray)
	{
		i->Destroy();
	}

	for (auto x : EquippedWeapon3PArray)
	{
		x->Destroy();
	}
}

# pragma endregion

# pragma endregion

void AC_PlayerCharacter::Client_LoadPlayerName_Implementation(const FString& SlotName)
{
	// If a custom name has been set, use it
	if (UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		if (UC_BaseSaveGame* LoadedGame = Cast<UC_BaseSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0)))
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, FString::Printf(TEXT("LOAD GAME SUCCEEDED (Slot name is: %s)"), *SlotName));
			Server_SetPlayerName(LoadedGame->SavedPlayerName);
		}
	}

	// If no custom name has been set use the steam name by default
	else
	{
		APlayerState* PS = UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayerState;
		if (PS)
		{
			Server_SetPlayerName(PS->GetPlayerName());
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, FString::Printf(TEXT("LOAD GAME FAILED, no name was saved name is: %s"), *PS->GetPlayerName()));
		}
	}
}

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

	DOREPLIFETIME(AC_PlayerCharacter, bCrouchKeyDown);
	DOREPLIFETIME(AC_PlayerCharacter, bIsHoldingGrenade);
	DOREPLIFETIME(AC_PlayerCharacter, EquippedGrenade);
	DOREPLIFETIME(AC_PlayerCharacter, DefaultMesh);


	DOREPLIFETIME_CONDITION(AC_PlayerCharacter, ControlRotation, COND_SkipOwner);
}

// INPUT
void AC_PlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// MOVING
	PlayerInputComponent->BindAxis("MoveForward", this, &AC_PlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AC_PlayerCharacter::MoveRight);

	// LOOKING
	//PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	//PlayerInputComponent->BindAxis("LookRight", this, &APawn::AddControllerYawInput);

	PlayerInputComponent->BindAxis("LookUp", this, &AC_PlayerCharacter::LookUp);
	PlayerInputComponent->BindAxis("LookRight", this, &AC_PlayerCharacter::LookRight);

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

	PlayerInputComponent->BindAction("Grenade", IE_Pressed, this, &AC_PlayerCharacter::ThrowGrenade);
	PlayerInputComponent->BindAction("Grenade", IE_Released, this, &AC_PlayerCharacter::ReleaseGrenade);

	PlayerInputComponent->BindAction("SwitchGrenades", IE_Pressed, this, &AC_PlayerCharacter::SwitchGrenades);

	//PlayerInputComponent->BindAction("Load", IE_Pressed, this, &AC_PlayerCharacter::LoadIt);


}

void AC_PlayerCharacter::LoadIt()
{
	APlayerState* PS = UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayerState;
	if (PS)
	{
		Client_LoadPlayerName(PS->UniqueId->ToString());
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("texthere: %s"), *PS->UniqueId->ToString()));
	}
}