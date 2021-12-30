// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloReach/UI/HUD/C_GamemodeHUDWidget.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "HaloReach/GameModes/C_BaseReachGameMode.h"
#include "HaloReach/GameModes/C_ReachGameStateBase.h"
#include "HaloReach/Player/PlayerExtra/C_ReachPlayerState.h"
#include "GameFramework/Character.h"

UC_GamemodeHUDWidget::UC_GamemodeHUDWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UC_GamemodeHUDWidget::NativeConstruct()
{
	
}

int UC_GamemodeHUDWidget::GetTopEnemyScore()
{
	TArray<APlayerState*> EnemeyStateArray;

	int HighestScore = 0;

	AC_ReachGameStateBase* GS = Cast<AC_ReachGameStateBase>(UGameplayStatics::GetGameState(GetWorld()));
	if(GS)
	{
		for (auto x : GS->PlayerArray)
		{
			EnemeyStateArray.AddUnique(x);
		}

		APlayerState* TPS = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetController()->GetPlayerState<APlayerState>();
		EnemeyStateArray.Remove(TPS);

		for (auto i : EnemeyStateArray)
		{
			AC_ReachPlayerState* PS = Cast<AC_ReachPlayerState>(i);
			if(PS)
			{
				if(PS->PlayerScore > HighestScore)
				{
					HighestScore = PS->PlayerScore;
					return HighestScore;
				}
			}
		}
	}

	return 0;
}

int UC_GamemodeHUDWidget::GetPlayerScore()
{
	AC_ReachPlayerState* PS = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetController()->GetPlayerState<AC_ReachPlayerState>();
	if(PS)
	{
		return PS->PlayerScore;
	}

	return 0;
}