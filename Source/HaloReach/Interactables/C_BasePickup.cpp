// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/Interactables/C_BasePickup.h"
#include "Components/BoxComponent.h"
#include "HaloReach/Interactables/Weapons/C_BaseWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "HaloReach/Player/C_PlayerCharacter.h"
#include <HaloReach/HaloReach.h>

// Sets default values
AC_BasePickup::AC_BasePickup()
{

	PickupMesh = CreateDefaultSubobject < USkeletalMeshComponent>(TEXT("Mesh Comp"));
	PickupMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	PickupMesh->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);
	PickupMesh->SetCollisionResponseToChannel(COLLISION_PICKUP, ECollisionResponse::ECR_Block);
	PickupMesh->SetSimulatePhysics(true);
	RootComponent = PickupMesh;

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	BoxComp->SetupAttachment(PickupMesh);

	bReplicates = true;
	SetReplicateMovement(true);

	bIsOverlapped = false;

}

void AC_BasePickup::BeginPlay()
{
	Super::BeginPlay();

	BoxComp->OnComponentBeginOverlap.AddDynamic(this, &AC_BasePickup::OnOverlapBegin);
	BoxComp->OnComponentEndOverlap.AddDynamic(this, &AC_BasePickup::OnOverlapEnd);
}

void AC_BasePickup::Interact_Implementation()
{
	if(bIsOverlapped)
	{
		OnInteract();
		Destroy();
	}
}

void AC_BasePickup::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Player = Cast<AC_PlayerCharacter>(OtherActor);

	//SHOW HUD MESSAGE
	if(OtherActor == Player)
	{
		bIsOverlapped = true;
		OnOverlap();
	}
}

void AC_BasePickup::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Player = Cast<AC_PlayerCharacter>(OtherActor);

	//SHOW HUD MESSAGE
	if (OtherActor == Player)
	{
		bIsOverlapped = false;
		OnOverlapEnd();
	}
}

void AC_BasePickup::OnOverlap()
{

}

void AC_BasePickup::OnOverlapEnd()
{

}

void AC_BasePickup::OnInteract()
{
	
}




