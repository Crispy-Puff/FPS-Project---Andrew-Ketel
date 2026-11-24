// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AWeapon.generated.h"

class UTexture2D;

class AAWeapon;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAmmoChanged, AAWeapon*, Weapon);

UCLASS()
class AAWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AAWeapon();
	
	// Public interface
	virtual void Fire();
	virtual void Reload();
	void RefillAmmo();
	int32 GetAmmoCount() const;
	
	// Additional ammo stuff for bots
	int32 GetTotalAmmo() const { return CurrentAmmo + AmmoHeld; }
	int32 GetMaxTotalAmmo() const { return MagazineSize + MaxAmmo; }
	float GetTotalAmmoFraction() const;
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	// For Observer pattern
	UPROPERTY(BlueprintAssignable, Category = "Weapon")
	FOnAmmoChanged OnAmmoChanged;

protected:
	// Accessible to subclasses
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	int32 MaxAmmo;
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	int32 MagazineSize = 10; // default
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float DamageAmount = 5; // default
	
	// Minimum time (seconds) that must pass between shots
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float TimeBetweenShots = 0.25f; // default
    
	// Timestamp of the last successful fire, in world seconds
	float LastFireTime = -1.0f; // start off-cooldown
	
	// Icon shown on the HUD when this weapon is equipped
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	class UTexture2D* WeaponIcon;
	

public:	
	// Internal state
        int32 CurrentAmmo;
		int32 AmmoHeld;
        bool bUnlimitedAmmo;

	// Debug ammo display
	void UpdateAmmoDisplay();
	
	// Formatted ammo string for UI, reused by both debug display and HUD widget
	FString GetAmmoDisplayText() const;

	UTexture2D* GetWeaponIcon() const { return WeaponIcon; }
};
