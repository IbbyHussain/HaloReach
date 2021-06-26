
#include "HaloReach/Interactables/C_MGPickup.h"

// Sets default values
AC_MGPickup::AC_MGPickup()
{
	/*WeaponSocket1P = ("MG_1P_Socket");
	WeaponSocket3P = ("MG_3P_Socket");*/
}

void AC_MGPickup::OnInteract()
{
	SpawnWeapon(WeaponClass);
}