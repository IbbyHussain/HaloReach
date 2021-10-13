
#include "HaloReach/Interactables/Weapons/Grenades/C_PlasmaGrenade.h"
#include "Components/BoxComponent.h"
#include "HaloReach/Player/C_PlayerCharacter.h"
#include "Components/CapsuleComponent.h"

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
			AC_PlayerCharacter* PlayerCharacter = Cast<AC_PlayerCharacter>(OtherActor);
			AActor* Akh = Cast<AActor>(OtherActor);
			
			// If we hit a player, Attach grenade to it - same for vechile 
			if (PlayerCharacter && OtherComp == PlayerCharacter->GetMesh3P() && OtherComp != PlayerCharacter->GetCapsuleComponent())
			{

				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("HIT PLAYER")));

				bStartExplosionOnce = true;

				// Separte start explosion and set simulate physics, otherwise grenade will stop simulating when it reaches capsule comp instead of mesh comp
				StartExplosion();

				MeshComp->SetSimulatePhysics(false);

				AttachToComponent(PlayerCharacter->GetMesh3P(), FAttachmentTransformRules::KeepWorldTransform, SweepResult.BoneName);
			}

			else if(Akh && Akh != PlayerCharacter)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("HIT GENERIC ACTOR")));


				bStartExplosionOnce = true;

				StartExplosion();

				MeshComp->SetSimulatePhysics(false);
			}

		}
	}
	
}

void AC_PlasmaGrenade::Thrown(FVector ImpulseDirection)
{
	Super::Thrown(ImpulseDirection);

	bHasGrenadeBeenThrown = true;

}