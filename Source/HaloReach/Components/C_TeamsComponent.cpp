
#include "HaloReach/Components/C_TeamsComponent.h"
#include "HaloReach/Player/C_PlayerCharacter.h"

UC_TeamsComponent::UC_TeamsComponent()
{

}

void UC_TeamsComponent::BeginPlay()
{
	Super::BeginPlay();

	//SetTeam(ETeam::RED);
	//UpdateOwnerColour();


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
			PlayerCharacter->SetPlayerColour(FColor::Orange);
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Player Team is: ORANGE")));
			break;

		case ETeam::PURPLE:
			PlayerCharacter->SetPlayerColour(FColor::Purple);
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Player Team is: PURPLE")));
			break;

		case ETeam::CYAN:
			PlayerCharacter->SetPlayerColour(FColor(0,255,255));
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Player Team is: WHITE")));
			break;

		case ETeam::YELLOW:
			PlayerCharacter->SetPlayerColour(FColor::Yellow);
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Player Team is: YELLOW")));
			break;

		case ETeam::NEUTRAL:
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Player Team is: NEUTRAL")));
			break;
		}
	}
}

TArray<ETeam> UC_TeamsComponent::GetAllTeams()
{
	for (ETeam T : TEnumRange<ETeam>())
	{
		TeamsArray.AddUnique(T);
	}
	return TeamsArray;
}

bool UC_TeamsComponent::IsSameTeam(ETeam Team1, ETeam Team2)
{
	if(Team1 == Team2)
	{
		return true;
	}

	return false;
}

void UC_TeamsComponent::OnTeamChanged()
{
	// If owner is a player, switch player colour
	AC_PlayerCharacter* PlayerCharacter = Cast<AC_PlayerCharacter>(GetOwner());
	if(PlayerCharacter)
	{

	}
}

void UC_TeamsComponent::Server_SetTeam_Implementation(ETeam NewTeam)
{
	Team = NewTeam;
}
