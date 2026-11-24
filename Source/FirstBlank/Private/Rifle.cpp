#include "Rifle.h"

ARifle::ARifle()
{
	bUnlimitedAmmo = false;
	MaxAmmo = 100;
	DamageAmount = 12.0f; 
	TimeBetweenShots = 0.1f; 
	
	AmmoHeld = MaxAmmo;
	MagazineSize = 32;
	CurrentAmmo = MagazineSize;
}

void ARifle::Fire()
{
	Super::Fire();
	//UE_LOG(LogTemp, Log, TEXT("Rifle fired!"));
}