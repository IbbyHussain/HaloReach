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
	Super::NativeConstruct();

	StoreWidgetAnimation();

	MovePlayerScoreAnimation = GetAnimationByName(TEXT("PlayerScoreBoxMove"));
	MoveEnemyScoreAnimation = GetAnimationByName(TEXT("EnemyScoreBoxMove"));

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
					//HighestEnemyScore = HighestScore;
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

void UC_GamemodeHUDWidget::StoreWidgetAnimation()
{
	AnimationsMap.Empty();

	UProperty* Prop = GetClass()->PropertyLink;

	while (Prop)
	{
		// only evaluate object properties, skip rest
		if (Prop->GetClass() == UObjectProperty::StaticClass())
		{
			UObjectProperty* ObjProp = Cast<UObjectProperty>(Prop);

			// only get back properties that are of type widget animation
			if (ObjProp->PropertyClass == UWidgetAnimation::StaticClass())
			{
				UObject* Obj = ObjProp->GetObjectPropertyValue_InContainer(this);
				// only get back properties that are of type widget animation
				UWidgetAnimation* WidgetAnimation = Cast<UWidgetAnimation>(Obj);
				// if casting worked update map with new animation
				if (WidgetAnimation && WidgetAnimation->MovieScene)
				{
					FName AnimName = WidgetAnimation->MovieScene->GetFName();
					//GEngine->AddOnScreenDebugMessage(-1, 4.5f, FColor::Magenta, AnimName.ToString());
					AnimationsMap.Add(AnimName, WidgetAnimation);
				}
			}
		}
		Prop = Prop->PropertyLinkNext;
	}
}

UWidgetAnimation* UC_GamemodeHUDWidget::GetAnimationByName(FName AnimationName) const
{
	UWidgetAnimation* const* WidgetAnimation = AnimationsMap.Find(AnimationName);

	if (WidgetAnimation)
	{
		return *WidgetAnimation;
	}

	return nullptr;
}

void UC_GamemodeHUDWidget::SwapScoreElements()
{
	AC_ReachPlayerState* PS = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetController()->GetPlayerState<AC_ReachPlayerState>();
	if(1)
	{
		PlayAnimation(MovePlayerScoreAnimation, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f);
		PlayAnimation(MoveEnemyScoreAnimation, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f);
	}
	
	else
	{
		PlayAnimation(MovePlayerScoreAnimation, 0.0f, 1, EUMGSequencePlayMode::Reverse, 1.0f);
		PlayAnimation(MoveEnemyScoreAnimation, 0.0f, 1, EUMGSequencePlayMode::Reverse, 1.0f);
	}
}

void UC_GamemodeHUDWidget::LostLead()
{
	PlayAnimation(MovePlayerScoreAnimation, 0.0f, 1, EUMGSequencePlayMode::Reverse, 1.0f);
	PlayAnimation(MoveEnemyScoreAnimation, 0.0f, 1, EUMGSequencePlayMode::Reverse, 1.0f);
}

void UC_GamemodeHUDWidget::GainedLead()
{
	PlayAnimation(MovePlayerScoreAnimation, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f);
	PlayAnimation(MoveEnemyScoreAnimation, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f);
}
