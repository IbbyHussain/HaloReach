// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/Libraries/C_DamageLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "HaloReach/Player/C_Playercharacter.h"


float UC_DamageLibrary::DealDamage(AActor* DamagedActor, float BaseDamage, AController* EventInstigator, AActor* DamageCauser)
{
	AC_PlayerCharacter* DamagedPlayer = Cast<AC_PlayerCharacter>(DamagedActor);
	AC_PlayerCharacter* DamageCauserPlayer = Cast<AC_PlayerCharacter>(DamagedActor);

	UGameplayStatics::ApplyDamage(DamagedActor, BaseDamage, EventInstigator, DamageCauser, NULL);

	return 0.0f;
}

void UC_DamageLibrary::DealDamageAOE()
{

}