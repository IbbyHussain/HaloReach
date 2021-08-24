// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/Interactables/Weapons/C_BaseWeapon.h"
#include "HaloReach/Player/C_PlayerCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "HaloReach/UI/HUD/C_PlayerHUD.h"
#include "HaloReach/UI/HUD/C_PlayerHUDWidget.h"
#include "HaloReach/UI/HUD/C_CrosshairWidget.h"


AC_BaseWeapon::AC_BaseWeapon()
{
	WeaponMesh = CreateDefaultSubobject < USkeletalMeshComponent>(TEXT("Mesh Comp"));
	RootComponent = WeaponMesh;
	WeaponMesh->SetOnlyOwnerSee(true);
	WeaponMesh->bCastDynamicShadow = false;
	WeaponMesh->CastShadow = false;

	Player = Cast<AC_PlayerCharacter>(GetOwner());

	bReplicates = true;

	SpreadTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("SpreadTimeline"));
	SpreadInterpFunction.BindUFunction(this, FName("SpreadTimelineFloatReturn"));
}

void AC_BaseWeapon::BeginPlay()
{
	Super::BeginPlay();

	Melee3PMontage->GetPlayLength();

	MeleeTime = Melee1PMontage->GetPlayLength() / Melee1PMontage->RateScale;

	if (FSpreadCurve)
	{
		SpreadTimeline->AddInterpFloat(FSpreadCurve, SpreadInterpFunction, FName("Alpha"));
		SpreadTimeline->SetLooping(false);
	}

}

UAnimMontage* AC_BaseWeapon::GetWeaponEquipMontage()
{
	return WeaponEquipAnimation;
}

UAnimMontage* AC_BaseWeapon::GetWeaponReloadMontage()
{
	return ReloadAnimation;
}

UAnimMontage* AC_BaseWeapon::GetWeaponFireMontage()
{
	return FireAnimation;
}

UAnimMontage* AC_BaseWeapon::GetWeapon1PMeleeMontage()
{
	return Melee1PMontage;
}

UAnimMontage* AC_BaseWeapon::GetWeapon3PEquipMontage()
{
	return Equip3PMontage;
}

UAnimMontage* AC_BaseWeapon::GetWeapon3PReloadMontage()
{
	return Reload3PMontage;
}

UAnimMontage* AC_BaseWeapon::GetWeapon3PFireMontage()
{
	return Fire3PMontage;
}

UAnimMontage* AC_BaseWeapon::GetWeapon3PMeleeMontage()
{
	return Melee3PMontage;
}

void AC_BaseWeapon::Attack()
{
	// This will not work for automatic weapons, as Attack calls startautofire() not fire()
	AddWeaponSpread();
}

void AC_BaseWeapon::StopAttack()
{

}

void AC_BaseWeapon::SpreadTimelineFloatReturn(float Value)
{
	AC_PlayerHUD* HUD = Cast<AC_PlayerHUD>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD());
	if(HUD)
	{
		HUD->HUDWidget->Crosshair->crosshair_spread = FMath::Lerp(CurrentSpread, DefaultWeaponSpread, Value);
	}
}

void AC_BaseWeapon::AddWeaponSpread()
{
	AC_PlayerHUD* HUD = Cast<AC_PlayerHUD>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD());
	if (HUD)
	{
		// increment by 5, then reset
		CurrentSpread = HUD->AddCrosshairSpreadHUD(SpreadAmount, DefaultWeaponSpread, MaxWeaponSpread);
		SpreadTimeline->PlayFromStart();
	}
}
