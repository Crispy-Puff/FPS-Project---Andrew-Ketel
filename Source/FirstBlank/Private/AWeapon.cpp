// Fill out your copyright notice in the Description page of Project Settings.


#include "AWeapon.h"
#include "DrawDebugHelpers.h"
#include "CharacterCodeTest.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AAWeapon::AAWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	
	CurrentAmmo = MagazineSize;
	AmmoHeld = MaxAmmo;
	
}

// Called when the game starts or when spawned
void AAWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAWeapon::Fire()
{
	// time used for cooldown comparisons
	const float CurrentTime = GetWorld()->GetTimeSeconds();
	
	if (CurrentTime - LastFireTime < TimeBetweenShots)
	{
		// Still on cooldown
		return;
	}
	
	if (CurrentAmmo <= 0)
	{
		return;
	}
	
	// only commit the timestamp once we know we're actually firing
	LastFireTime = CurrentTime; 
	
	// debug display
	CurrentAmmo--;
	//UpdateAmmoDisplay();
	
	FVector Location;
	FRotator Rotation;
	GetOwner()->GetActorEyesViewPoint(Location, Rotation);

	FVector End = Location + (Rotation.Vector() * 10000.0f);

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this); // ignore weapon
	Params.AddIgnoredActor(GetOwner()); // ignore owner

	bool bHitSomething = GetWorld()->LineTraceSingleByChannel(Hit, Location, End, ECC_Visibility, Params);

	// Debug line: green if it hit something, red if it went the full distance
	FVector LineEnd = bHitSomething ? Hit.ImpactPoint : End;
	DrawDebugLine(
		GetWorld(),
		Location,
		LineEnd,
		bHitSomething ? FColor::Green : FColor::Red,
		false,      // persistent lines (false = disappears after Duration)
		2.0f,       // duration in seconds
		0,          // depth priority
		1.5f        // thickness
	);

	if (bHitSomething)
	{
		AActor* HitActor = Hit.GetActor(); // CHANGE TO CHARACTER (?)
		
		if (ACharacterCodeTest* HitCharacter = Cast<ACharacterCodeTest>(HitActor))
		{
			HitCharacter->ReceiveDamage(DamageAmount, GetOwner());
		}

		// mark the impact point itself
		DrawDebugPoint(GetWorld(), Hit.ImpactPoint, 10.0f, FColor::Yellow, false, 2.0f);
	}
	
	// Broadcast
	OnAmmoChanged.Broadcast(this);
}

void AAWeapon::Reload()
{
	//GEngine->AddOnScreenDebugMessage(-2, 5.0f, FColor::Orange, TEXT("Reloading!"));
    if (!bUnlimitedAmmo)
    {
    	int tempAmmo = MagazineSize - CurrentAmmo;
	
    	if (tempAmmo <= AmmoHeld)
    	{
    		CurrentAmmo = MagazineSize;
    		AmmoHeld -= tempAmmo;
    	}
    	else if (AmmoHeld > 0 && AmmoHeld < tempAmmo)
    	{
    		CurrentAmmo += AmmoHeld;
    		AmmoHeld = 0;
    	}
	}
	else
	{
		CurrentAmmo = MagazineSize;
	}
	
	// Broadcast
	OnAmmoChanged.Broadcast(this);
}

void AAWeapon::RefillAmmo()
{
	CurrentAmmo = MagazineSize;
	AmmoHeld = MaxAmmo;
	
	// Broadcast
	OnAmmoChanged.Broadcast(this);
}

int32 AAWeapon::GetAmmoCount() const
{
	return bUnlimitedAmmo ? -1 : CurrentAmmo;
}

FString AAWeapon::GetAmmoDisplayText() const
{
	return bUnlimitedAmmo
		? FString::Printf(TEXT("Ammo: %d / %d [Unlimited]"), CurrentAmmo, MagazineSize)
		: FString::Printf(TEXT("Ammo: %d / %d [%d]"), CurrentAmmo, MagazineSize, AmmoHeld);
}

void AAWeapon::UpdateAmmoDisplay()
{
	if (!GEngine) return;

	// Fixed key (1) means this message updates in place instead of stacking new lines. Remember for future.
	GEngine->AddOnScreenDebugMessage(1, 3.0f, FColor::Cyan, GetAmmoDisplayText());
}

float AAWeapon::GetTotalAmmoFraction() const
{
	if (bUnlimitedAmmo) return 1.0f;
	const int32 Max = GetMaxTotalAmmo();
	return Max > 0 ? static_cast<float>(GetTotalAmmo()) / Max : 0.0f;
}


