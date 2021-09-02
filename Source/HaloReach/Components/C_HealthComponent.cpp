
#include "HaloReach/Components/C_HealthComponent.h"
#include "C_HealthComponent.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"
#include "HaloReach/UI/HUD/C_PlayerHUD.h"
#include "Kismet/GameplayStatics.h"
#include "HaloReach/Player/C_PlayerCharacter.h"


UC_HealthComponent::UC_HealthComponent()
{
	MaxHealth = 45.0f;
	MaxShields = 70.0f;
}

void UC_HealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UC_HealthComponent, Health);
	DOREPLIFETIME(UC_HealthComponent, Shields);
}

void UC_HealthComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		MyOwner->OnTakeAnyDamage.AddDynamic(this, &UC_HealthComponent::HandleTakeAnyDamage);
	}

	Health = MaxHealth;
	Shields = MaxShields;

}

// HANDLE DAMAGE 

// THIS ONLY EXECUTES ON SERVER --- this is for anti cheat purposes
void UC_HealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	// calls client RPC so code is EXECUTED on SERVER but CALLED on CLIENT --> As this function only executes on server
	Client_Test(Damage, DamageCauser); // An OnRep would not work in this case as if logic for being damaged is inside OnRep function, the HandleAnyDamage function does not work(no logic exeuted in it)and Apply damage would not damage player

}

// Works on both client and server
void UC_HealthComponent::Client_Test_Implementation(float Damage, AActor* Killer)
{
	if (Shields != 0.0f)
	{
		Shields -= Damage;

		// This ensures that if the amount of damage exceeds the amount of shields, the leftover damage is dealt to Health
		// E.g shields = 70 and Health = 45. If damage is 100, shields = 0 and health = 15 - NO LONGER USED - KEPT FOR FUTURE REFERENCE
		if (Shields < 0.0f)
		{
			//Shields *= -1.0f;
			//Health = FMath::Clamp(Health - Shields, 0.0f, MaxHealth);
			Shields = 0.0f;
		}
	}

	else
	{
		Health = FMath::Clamp(Health - Damage, 0.0f, MaxHealth);
	}

	// Successfully tells game when player has died and sends correct killer name
	if (Health <= 0.0f)
	{
		//Server_Test(10.0f, Killer);
	}

	Server_Test(10.0f, Killer);

	//AC_PlayerCharacter* PlayerCharacter = Cast<AC_PlayerCharacter>(GetOwner());
	//PlayerCharacter->KillerName = LocalKiller->GetName();

	//AC_PlayerCharacter* PlayerCharacter = Cast<AC_PlayerCharacter>(GetOwner());
	//PlayerCharacter->KillerName = Killer->GetName();

	// These delegates will call HUD updates and update the combat state in the player class
	OnHealthChanged.Broadcast(this, Health, true, Killer); // killer 
	OnShieldsChanged.Broadcast(this, Shields);
}

void UC_HealthComponent::Server_Test_Implementation(float Damage, AActor* Killer)
{
	// gives correct data
	if(Killer)
	{
		AC_PlayerCharacter* PlayerCharacter = Cast<AC_PlayerCharacter>(GetOwner());
		PlayerCharacter->KillerName = Killer->GetName();
		Client_Name(Killer);
	}
}

void UC_HealthComponent::Client_Name_Implementation(AActor* Killer)
{
	AC_PlayerCharacter* PlayerCharacter = Cast<AC_PlayerCharacter>(GetOwner());
	PlayerCharacter->KillerName = Killer->GetName();
}

// SHIELD REGENERATION

void UC_HealthComponent::StartShieldRegeneration()
{
	if (Shields == MaxShields)
	{
		return;
	}

	else if (Shields < 0.0f)
	{
		Shields = 0.0f;
	}

	GetWorld()->GetTimerManager().SetTimer(ShieldRegenHandle, this, &UC_HealthComponent::RechargeShields, 0.25f, true, 0.0f);

	OnShieldsChanged.Broadcast(this, Shields);
}

void UC_HealthComponent::RechargeShields()
{
	if (Health != 0.0f)
	{
		// Recharge shields until shields are full
		if (Shields < MaxShields)
		{
			Shields = FMath::Clamp(Shields + 1.0f, 0.0f, MaxShields);
		}

		else
		{
			StopShieldRegeneration();
		}
	}

	OnShieldsChanged.Broadcast(this, Shields);
}

void UC_HealthComponent::StopShieldRegeneration()
{
	GetWorld()->GetTimerManager().ClearTimer(ShieldRegenHandle);
}

// HEALTH REGENERATION

void UC_HealthComponent::StartHealthRegeneration()
{
	if(Health == MaxHealth)
	{
		StopHealthRegeneration();
		StartShieldRegeneration();
		return;
	}

	GetWorld()->GetTimerManager().SetTimer(HealthRegenHandle, this, &UC_HealthComponent::RechargeHealth, 1.0f, true, 0.0f);
}

void UC_HealthComponent::RechargeHealth()
{
	// Health regenerates to the next section of Health, where there are 3 sections
	// E.g if Health is greater than 25 health will regenerate until full
	if((Health <= 45.0f) && (Health > 25.0f))
	{
		Health = FMath::Clamp(Health + 5.0f, 25.0f, MaxHealth);

		if(Health == 45)
		{
			StartShieldRegeneration();
			StopHealthRegeneration();
		}
	}

	// If health is greater than 10, health will regen until health is 25.
	else if((Health <= 25.0f) && (Health > 10.0f))
	{
		if(Health == 25)
		{
			StartShieldRegeneration();
			StopHealthRegeneration();
		}

		Health = FMath::Clamp(Health + 5.0f, 10.0f, 25.0f);
	}

	// If health is greater than 0, health will regen until health is 10.
	else if((Health <= 10.0f) && (Health > 0.0f))
	{
		if (Health == 10.0f)
		{
			StartShieldRegeneration();
			StopHealthRegeneration();
		}

		Health = FMath::Clamp(Health + 5.0f, 0.0f, 10.0f);
	}

	OnHealthChanged.Broadcast(this, Health, false, nullptr);
}

void UC_HealthComponent::StopHealthRegeneration()
{
	GetWorld()->GetTimerManager().ClearTimer(HealthRegenHandle);
}

// GETTER METHOD DEFINITIONS

float UC_HealthComponent::GetHealth()
{
	return Health;
}

float UC_HealthComponent::GetShields()
{
	return Shields;
}

