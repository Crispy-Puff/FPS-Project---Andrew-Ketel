// Fill out your copyright notice in the Description page of Project Settings.


#include "Pistol.h"

APistol::APistol()
{
	bUnlimitedAmmo = true;
	MaxAmmo = -1; // irrelevant since unlimited
	DamageAmount = 3; // check against brief later.
	TimeBetweenShots = 0.33f;
	
	MagazineSize = 16;
	CurrentAmmo = MagazineSize;
}

void APistol::Fire()
{
	Super::Fire();

	//UE_LOG(LogTemp, Log, TEXT("Pistol fired!"));
}