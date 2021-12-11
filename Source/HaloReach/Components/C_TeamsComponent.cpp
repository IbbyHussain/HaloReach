
#include "HaloReach/Components/C_TeamsComponent.h"
#include "HaloReach/Player/C_PlayerCharacter.h"

UC_TeamsComponent::UC_TeamsComponent()
{

}

void UC_TeamsComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UC_TeamsComponent::UpdateOwnerColour()
{
	AC_PlayerCharacter* PlayerCharacter = Cast<AC_PlayerCharacter>(GetOwner());
	if (PlayerCharacter)
	{
		switch (Team)
		{
		case ETeam::RED:
			PlayerCharacter->SetPlayerColour(FColor::Red);
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Player Team is: RED")));
			break;

		case ETeam::BLACK:
			PlayerCharacter->SetPlayerColour(FColor::Black);
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Player Team is: BLACK")));
			break;

		case ETeam::BLUE:
			PlayerCharacter->SetPlayerColour(FColor::Blue);
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Player Team is: BLUE")));
			break;

		case ETeam::GREEN:
			PlayerCharacter->SetPlayerColour(FColor::Green);
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Player Team is: GREEN")));
			break;

		case ETeam::ORANGE:
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Player Team is: ORANGE")));
			break;

		case ETeam::PURPLE:
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Player Team is: PURPLE")));
			break;

		case ETeam::WHITE:
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Player Team is: WHITE")));
			break;

		case ETeam::YELLOW:
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Player Team is: YELLOW")));
			break;

		case ETeam::NEUTRAL:
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Player Team is: NEUTRAL")));
			break;
		}
	}
}

TArray<FString> UC_TeamsComponent::GetAllTeams()
{
	for(ETeam T : TEnumRange<ETeam>())
	{
		FString TeamString = UEnum::GetValueAsString(T);
		TeamString.RemoveFromStart("ETeam::", ESearchCase::IgnoreCase);
		TeamsArray.AddUnique(TeamString);
	}

	for (auto x: TeamsArray)
	{
		UE_LOG(LogTemp, Log, TEXT(" All Teams: %s"), *x);
	}

	return TeamsArray;
}

TArray<ETeam> UC_TeamsComponent::GetAllTeamsENUM()
{
	for (ETeam T : TEnumRange<ETeam>())
	{
		TeamsENUMArray.AddUnique(T);
	}
	return TeamsENUMArray;
}

void UC_TeamsComponent::OnTeamChanged()
{
	// If owner is a player, switch player colour
	AC_PlayerCharacter* PlayerCharacter = Cast<AC_PlayerCharacter>(GetOwner());
	if(PlayerCharacter)
	{

	}
}

