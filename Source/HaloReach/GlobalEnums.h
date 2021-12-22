
#pragma once

UENUM(Blueprintable)
enum class EWeaponType : uint8
{
	RIFLE,
	PISTOL,
};

// used in team comp and in player state
UENUM()
enum class ETeam : uint8
{
	RED,
	BLUE,
	GREEN,
	YELLOW,
	ORANGE,
	PURPLE,
	CYAN,
	BLACK,
	NEUTRAL
};