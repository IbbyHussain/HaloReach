
#include "HaloReach/Interactables/Weapons/Grenades/C_PlasmaGrenade.h"
#include "Components/BoxComponent.h"
#include "HaloReach/Player/C_PlayerCharacter.h"

AC_PlasmaGrenade::AC_PlasmaGrenade()
{
	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Comp"));
	BoxComp->SetupAttachment(MeshComp);

	bHasGrenadeBeenThrown = false;

	bStartExplosionOnce = false;
}

void AC_PlasmaGrenade::BeginPlay()
{
	Super::BeginPlay();

	BoxComp->OnComponentBeginOverlap.AddDynamic(this, &AC_PlasmaGrenade::OnOverlapBegin);

}

void AC_PlasmaGrenade::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!bStartExplosionOnce)
	{
		// if we hit something - general object etc
		if (OtherActor && OtherActor != GetOwner() && bHasGrenadeBeenThrown)
		{
			bStartExplosionOnce = true;

			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Plasma Stick")));

			StartExplosion();

			MeshComp->SetSimulatePhysics(false);

			AC_PlayerCharacter* PlayerCharacter = Cast<AC_PlayerCharacter>(OtherActor);

			// If we hit a player, Attach grenade to it - same for vechile 
			if(PlayerCharacter)
			{
				AttachToComponent(PlayerCharacter->GetMesh3P(), FAttachmentTransformRules::KeepWorldTransform, SweepResult.BoneName);
			}
		}
	}
	
}

void AC_PlasmaGrenade::Thrown(FVector ImpulseDirection)
{
	Super::Thrown(ImpulseDirection);

	bHasGrenadeBeenThrown = true;

}