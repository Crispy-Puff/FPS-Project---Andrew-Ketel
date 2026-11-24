// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "BodyStates.h"
#include "CharacterCodeTest.generated.h"

// Forward declarations
class UCameraComponent;
class UChildActorComponent;
class AAWeapon;
class UTexture2D;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStatChanged, float, Current, float, Max);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponEvent, AAWeapon*, Weapon);

UCLASS()
class ACharacterCodeTest : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACharacterCodeTest();
	
	virtual void AddMovementInput(FVector WorldDirection, float ScaleValue = 1.0f, bool bForce = false) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly)
	USceneComponent* SpawnLocation;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	virtual void Jump() override; // override
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	virtual void ReceiveDamage(int DamageAmount, AActor* DamageInstigator = nullptr);
	
	// Normal walking speed of character
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player | Movement", meta = (AllowPrivateAccess = "true"))
	float NormalMaxWalkSpeed = 1000.0f; // Default 400
	
	// Sprinting speed of character
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player | Movement", meta = (AllowPrivateAccess = "true"))
	float RunningMaxWalkSpeed = 1800.0f; // Default 1000
	
	// Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player | Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* CameraComponent;
	
	// Weaponry
	UPROPERTY(EditDefaultsOnly, Category = "Player | Weapon")
	TSubclassOf<AAWeapon> DefaultWeaponClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Player | Weapon")
	TSubclassOf<AAWeapon> RifleWeaponClass;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player | Weapon")
	AAWeapon* PistolWeapon = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player | Weapon")
	AAWeapon* RifleWeapon = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player | Weapon")
	AAWeapon* ActiveWeapon = nullptr;

	void EquipWeapon(AAWeapon* NewWeapon);
	
	// Movement states
	UFUNCTION(BlueprintCallable, Category = "Player | Movement")
	void SetRunning(bool IsRunning);
	
	UFUNCTION(BlueprintCallable, Category = "Player | Movement")
	void ToggleRunning();
	
	// Called to indicate player has jumped since last update
	UFUNCTION(BlueprintCallable, Category = "Player | Movement")
	void SetHasJumped();
	
	// Called to indicate player has sprinted since last update
	UFUNCTION(BlueprintCallable, Category = "Player | Movement")
	void SetHasRan();
	
	// Weapon related goods
	UFUNCTION(BlueprintCallable, Category = "Player | Weapon")
	void Fire();
	
	UFUNCTION(BlueprintCallable, Category = "Player | Weapon")
	void Reload();
	
	UFUNCTION(BlueprintCallable, Category = "Player | Weapon")
	void SwitchToWeaponSlot(int32 SlotIndex);

	// Called by the rifle pickup actor on overlap
	UFUNCTION(BlueprintCallable, Category = "Player | Weapon")
	void PickupRifle();
	
	UFUNCTION(BlueprintCallable, Category = "Player | Weapon")
	FString GetActiveWeaponAmmoText() const;
	
	UFUNCTION(BlueprintCallable, Category = "Player | Weapon")
	UTexture2D* GetActiveWeaponIcon() const;
	
	// Health
	UFUNCTION(BlueprintCallable, Category = "Player | Heal")
	void Heal();
    
	UFUNCTION(BlueprintCallable, Category = "Player | Stats")
	int32 GetCurrentHealth() const { return currHealth; }
    
	UFUNCTION(BlueprintCallable, Category = "Player | Stats")
	int32 GetMaxHealth() const { return maxHealth; }
    
	// Stamina
	UFUNCTION(BlueprintCallable, Category = "Player | Stats")
	float GetCurrentStamina() const { return CurrStamina; }
    
	UFUNCTION(BlueprintCallable, Category = "Player | Stats")
	float GetMaxStamina() const { return MaxStamina; }
	
	// Added stopgap to ensure no wasted stamina in air on repeat jump attempts.
	virtual void Landed(const FHitResult& Hit) override;
	
	// Respawning and death
	bool IsDead() const { return currHealth <= 0; }
	//void Die(); 
	void Die(AActor* Killer = nullptr);
	void Respawn();
	
	// Weapons for bots
	bool HasRifle() const { return bHasRifle; }
	bool NeedsReload() const;
	void SelectBestWeapon();
	float GetRifleTotalAmmoFraction() const;
	
	// For broadcasting
	UPROPERTY(BlueprintAssignable, Category = "Player | Events")
	FOnStatChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Player | Events")
	FOnStatChanged OnStaminaChanged;

	UPROPERTY(BlueprintAssignable, Category = "Player | Events")
	FOnWeaponEvent OnActiveWeaponChanged;   // Equipped a different weapon

	UPROPERTY(BlueprintAssignable, Category = "Player | Events")
	FOnWeaponEvent OnActiveAmmoChanged;     // The active weapon's ammo changed

	AAWeapon* GetActiveWeapon() const { return ActiveWeapon; }
	
	// Leaderboard junk
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player | Identity")
	FString DisplayName = TEXT("Player");
	
	//  Body state machine API (if you can call it that)
	void SetBodyState(EBodyState NewState);
	const TCHAR* GetBodyStateName() const;

	// Things controller calls
	void RequestCrouchToggle();

	void ApplyRunning(bool bRun);
	void StartCrouch();
	void StopCrouch();
	void ApplyDeathEffects();
	void TryJump();

	bool CanSprint() const;
	bool CanJump() const;
	void SetExhausted(bool bValue) { bExhausted = bValue; }

	void DrainStamina()  { CurrStamina -= RunStamina; }
	void RegenStamina()  { if (CurrStamina < MaxStamina) CurrStamina += RestStamina; }
	bool IsStaminaDepleted() const { return CurrStamina <= RunStamina; }

	// Stamina needed again before sprinting or jumping after exhaustion
	UPROPERTY(EditDefaultsOnly, Category = "Player | Stamina")
	float ExhaustionRecoverStamina = 20.0f;
	
private:
	bool bIsRunning = false;
	bool bHasRan = false;
	bool bHasJumped = false;
	bool bHasRifle = false;
	
	AActor* spawnedActor = nullptr;
	
	// Health values
	static constexpr int BaseStatVal = 100;
	int maxHealth = BaseStatVal;
	int currHealth = BaseStatVal;
	
	// stamina stuff
	static constexpr float MaxStamina = 100.0f;
	static constexpr float JumpStamina = 35.0f;
	static constexpr float RunStamina = 1.5f;
	static constexpr float RestStamina = 4.0f; 
	float CurrStamina = MaxStamina;
	float RecoupStaminaFactor = 1.0f; 
	// Default stamina amounts: [100, 24, 5, 4, 1]
	
	
	// Respawn details
	FTimerHandle RespawnTimer;
	UPROPERTY(EditDefaultsOnly, Category = "Player | Respawn")
	float RespawnDelay = 3.0f;
	
	// More broadcasting stuff
	UFUNCTION()
	void HandleWeaponAmmoChanged(AAWeapon* Weapon);
	float LastBroadcastStamina = -1.0f;
	
	// More state stuff
	FBodyState* GetBodyState() const { return static_cast<FBodyState*>(BodyMachine.GetCurrent()); }

	TStateMachine<ACharacterCodeTest> BodyMachine;
	FBodyNormalState    NormalState;
	FBodySprintingState SprintingState;
	FBodyCrouchingState CrouchingState;
	FBodyDeadState      DeadState;

	bool bExhausted = false;
	
	// New "Factory" system for adding weapons
	AAWeapon* CreateWeapon(TSubclassOf<AAWeapon> WeaponClass);
	
};
