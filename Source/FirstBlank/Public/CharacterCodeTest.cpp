// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterCodeTest.h"
#include "Camera/CameraComponent.h"
#include "AWeapon.h"        
#include "TimerManager.h"

#include "GameFramework/Controller.h"
#include "Camera/CameraTypes.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/Engine.h"
#include "ArenaGameState.h"

// Sets default values
ACharacterCodeTest::ACharacterCodeTest()
{
	PrimaryActorTick.bCanEverTick = true;
	
	GetCharacterMovement()->MaxWalkSpeed = NormalMaxWalkSpeed;
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(GetMesh(), TEXT("head")); 
	CameraComponent->bUsePawnControlRotation = true;
	
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
	
	SetActorTickInterval((0.05f));
	SetActorTickEnabled(true);
	
}

void ACharacterCodeTest::AddMovementInput(FVector WorldDirection, float ScaleValue, bool bForce)
{
	// Don't think this works. meh. Check tick instead.
	if (bIsRunning && CurrStamina <= 0)
	{
		SetRunning(false);
	}
	
	// This does work
	Super::AddMovementInput(WorldDirection, ScaleValue, bForce);
}

// Called when the game starts or when spawned
void ACharacterCodeTest::BeginPlay()
{
	Super::BeginPlay();
	SetBodyState(EBodyState::Normal);
	
	PistolWeapon = CreateWeapon(DefaultWeaponClass);
	if (PistolWeapon)
	{
		ActiveWeapon = PistolWeapon;
		//ActiveWeapon->UpdateAmmoDisplay();
		PistolWeapon->OnAmmoChanged.AddDynamic(this, &ACharacterCodeTest::HandleWeaponAmmoChanged);
		OnActiveWeaponChanged.Broadcast(ActiveWeapon);
	}
	// Add to leaderboard
	if (AArenaGameState* GS = GetWorld()->GetGameState<AArenaGameState>())
	{
		GS->RegisterCombatant(this, DisplayName);
	}
	
}

// Called every frame
void ACharacterCodeTest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	bHasRan = false;
	//bHasJumped = false; <- moved to separate method
	
	// Sprinting drains stamina
	BodyMachine.Tick(*this, DeltaTime);   

	if (CurrStamina > MaxStamina) CurrStamina = MaxStamina;
	if (CurrStamina < 0.0f)       CurrStamina = 0.0f;
	if (bExhausted && CurrStamina >= ExhaustionRecoverStamina) bExhausted = false;
	
	// Stamina broadcast
	if (!FMath::IsNearlyEqual(CurrStamina, LastBroadcastStamina))
	{
		LastBroadcastStamina = CurrStamina;
		OnStaminaChanged.Broadcast(CurrStamina, MaxStamina);
	}
}

void ACharacterCodeTest::ReceiveDamage(int DamageAmount, AActor* DamageInstigator)
{
	// Prevent any mishaps
	if (IsDead())
		return;
	
	currHealth -= DamageAmount;
	
	if (currHealth < 0)
		currHealth = 0;
	
	if (currHealth <= 0)
	{
		Die(DamageInstigator);
	}
	
	OnHealthChanged.Broadcast(currHealth, maxHealth);
}

void ACharacterCodeTest::RequestCrouchToggle()
{
	// Realised crouch just didn't work before with override. Testing this.
	if (FBodyState* S = GetBodyState()) S->OnCrouchToggle(*this);
}

void ACharacterCodeTest::Jump()
{
	if (FBodyState* S = GetBodyState()) S->OnJump(*this);
}

// Called to bind functionality to input
void ACharacterCodeTest::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ACharacterCodeTest::SetRunning(bool IsRunning)
{
	if (FBodyState* S = GetBodyState()) S->OnSprintSet(*this, IsRunning);
}

void ACharacterCodeTest::ToggleRunning()
{
	SetRunning(!bIsRunning);
}

void ACharacterCodeTest::SetHasJumped()
{
	bHasJumped = true;
}

void ACharacterCodeTest::SetHasRan()
{
	bHasRan = true;
}

void ACharacterCodeTest::Fire()
{
	if (IsDead()) return;
	
	if (ActiveWeapon)
	{
		ActiveWeapon->Fire();
		SetRunning(false);
		
		HandleWeaponAmmoChanged(ActiveWeapon);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Weapon is NULL in Fire()!"));
	}
}

void ACharacterCodeTest::Reload()
{
	if (IsDead()) return;
	
	if (ActiveWeapon)
	{
		ActiveWeapon->Reload();
		HandleWeaponAmmoChanged(ActiveWeapon);
	}
}

void ACharacterCodeTest::EquipWeapon(AAWeapon* NewWeapon)
{
	// Shouldn't need the dead guard here since rifle is removed, but kept in case.
	if (IsDead()) return;
	
	if (!NewWeapon || NewWeapon == ActiveWeapon)
	{
		return;
	}

	if (ActiveWeapon)
	{
		ActiveWeapon->SetActorHiddenInGame(true);
	}

	ActiveWeapon = NewWeapon;
	ActiveWeapon->SetActorHiddenInGame(false);
	
	// debug
	//ActiveWeapon->UpdateAmmoDisplay();
	
	OnActiveWeaponChanged.Broadcast(ActiveWeapon);
	//UpdateAmmoHUD(); 
	//UpdateWeaponIconHUD(); 
}

void ACharacterCodeTest::SwitchToWeaponSlot(int32 SlotIndex)
{
	switch (SlotIndex)
	{
	case 1:
		if (PistolWeapon)
		{
			EquipWeapon(PistolWeapon);
		}
		break;

	case 2:
		if (bHasRifle && RifleWeapon)
		{
			EquipWeapon(RifleWeapon);
		}
		else if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(3, 2.0f, FColor::Orange,
				TEXT("No rifle picked up yet!"));
		}
		break;

	default:
		break;
	}
}

void ACharacterCodeTest::PickupRifle()
{
	// Much more simplified after factory addition
	
	if (!bHasRifle)
	{
		RifleWeapon = CreateWeapon(RifleWeaponClass);
		if (RifleWeapon)
		{
			bHasRifle = true;
			EquipWeapon(RifleWeapon); // auto-swap 
		}
	}
	else
	{
		RifleWeapon->RefillAmmo();
	}
}

void ACharacterCodeTest::Heal()
{
	// Don't want to cause weird things to happen if entities die on a pickup.
	if (IsDead()) return;
	
	currHealth += 25;
	if (currHealth > maxHealth)
		currHealth = maxHealth;
	
	OnHealthChanged.Broadcast(currHealth, maxHealth);
}

FString ACharacterCodeTest::GetActiveWeaponAmmoText() const
{
	return ActiveWeapon ? ActiveWeapon->GetAmmoDisplayText() : TEXT("No Weapon");
}

UTexture2D* ACharacterCodeTest::GetActiveWeaponIcon() const
{
	return ActiveWeapon ? ActiveWeapon->GetWeaponIcon() : nullptr;
}

void ACharacterCodeTest::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	
	bHasJumped = false;
}

// bot logic tool
bool ACharacterCodeTest::NeedsReload() const
{
	return ActiveWeapon && ActiveWeapon->CurrentAmmo <= 0;
}

// bot logic tool
void ACharacterCodeTest::SelectBestWeapon()
{
	if (bHasRifle && RifleWeapon && RifleWeapon->GetTotalAmmo() > 0)
	{
		EquipWeapon(RifleWeapon);
	}
	else if (PistolWeapon)
	{
		EquipWeapon(PistolWeapon);
	}
}

float ACharacterCodeTest::GetRifleTotalAmmoFraction() const
{
	return (bHasRifle && RifleWeapon) ? RifleWeapon->GetTotalAmmoFraction() : 0.0f;
}


void ACharacterCodeTest::Die(AActor* Killer)
{
	if (AArenaGameState* GS = GetWorld()->GetGameState<AArenaGameState>())
	{
		GS->RecordDeath(this, Killer);
	}
	
	
	SetBodyState(EBodyState::Dead);

	// Weapons are separate actors
	TArray<AActor*> Attached;
	GetAttachedActors(Attached);
	for (AActor* A : Attached)
	{
		A->SetActorHiddenInGame(true);
	}

	GetWorldTimerManager().SetTimer(RespawnTimer, this, &ACharacterCodeTest::Respawn, RespawnDelay, false);
}

void ACharacterCodeTest::Respawn()
{
	// Pick a random respawn point
	TArray<AActor*> Starts;
	UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), Starts);
	if (Starts.Num() > 0)
	{
		const AActor* Spot = Starts[FMath::RandRange(0, Starts.Num() - 1)];
		SetActorLocationAndRotation(Spot->GetActorLocation(), Spot->GetActorRotation(),
			false, nullptr, ETeleportType::TeleportPhysics);
	}

	// Reset stats
	currHealth = maxHealth;
	CurrStamina = MaxStamina;
	
	bExhausted = false;
	SetBodyState(EBodyState::Normal);   // Normal::Enter restores walking speed

	// Reset loadout: lose the rifle, refill the pistol
	if (RifleWeapon)
	{
		RifleWeapon->Destroy();
		RifleWeapon = nullptr;
	}
	bHasRifle = false;
	ActiveWeapon = PistolWeapon;
	if (PistolWeapon)
	{
		PistolWeapon->RefillAmmo();
		PistolWeapon->SetActorHiddenInGame(false);
	}

	// Re-enable the character
	SetActorEnableCollision(true);
	SetActorHiddenInGame(false);
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	
	// Broadcasts replaces HUD update methods
	OnHealthChanged.Broadcast(currHealth, maxHealth);
	OnActiveWeaponChanged.Broadcast(ActiveWeapon);
	HandleWeaponAmmoChanged(ActiveWeapon);
	
}

void ACharacterCodeTest::HandleWeaponAmmoChanged(AAWeapon* Weapon)
{
	if (Weapon == ActiveWeapon)
	{
		OnActiveAmmoChanged.Broadcast(Weapon);
	}
}

// here be actual dragons (imagine them)
void ACharacterCodeTest::SetBodyState(EBodyState NewState)
{
	FBodyState* Next = nullptr;
	switch (NewState)
	{
	case EBodyState::Normal:    Next = &NormalState;    break;
	case EBodyState::Sprinting: Next = &SprintingState; break;
	case EBodyState::Crouching: Next = &CrouchingState; break;
	case EBodyState::Dead:      Next = &DeadState;      break;
	}
	BodyMachine.ChangeState(*this, Next);
}

const TCHAR* ACharacterCodeTest::GetBodyStateName() const
{
	return BodyMachine.GetCurrent() ? BodyMachine.GetCurrent()->GetName() : TEXT("None");
}

void ACharacterCodeTest::ApplyRunning(bool bRun)
{
	bIsRunning = bRun;
	GetCharacterMovement()->MaxWalkSpeed = bIsRunning ? RunningMaxWalkSpeed : NormalMaxWalkSpeed;
}

void ACharacterCodeTest::StartCrouch() { Super::Crouch(false); }
void ACharacterCodeTest::StopCrouch()  { Super::UnCrouch(); }

bool ACharacterCodeTest::CanSprint() const
{
	return !IsDead() && !bExhausted && CurrStamina > RunStamina;
}

bool ACharacterCodeTest::CanJump() const
{
	return !IsDead() && !bExhausted && CurrStamina >= JumpStamina
		&& !GetCharacterMovement()->IsFalling();
}

void ACharacterCodeTest::TryJump()
{
	if (!CanJump()) return;
	CurrStamina -= JumpStamina;
	bHasJumped = true;
	Super::Jump();
}

// The old body of Die(), without the scoreboard record and the respawn timer
void ACharacterCodeTest::ApplyDeathEffects()
{
	GetCharacterMovement()->DisableMovement();
	SetActorEnableCollision(false);
	SetActorHiddenInGame(true);

	TArray<AActor*> Attached;
	GetAttachedActors(Attached);
	for (AActor* A : Attached)
	{
		A->SetActorHiddenInGame(true);
	}
}

// Factory
AAWeapon* ACharacterCodeTest::CreateWeapon(TSubclassOf<AAWeapon> WeaponClass)
{
	if (!WeaponClass)
	{
		UE_LOG(LogTemp, Error, TEXT("%s: CreateWeapon called with no weapon class"), *GetName());
		return nullptr;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;               // Fire() reads its aim from GetOwner()
	SpawnParams.Instigator = GetInstigator();

	AAWeapon* Weapon = GetWorld()->SpawnActor<AAWeapon>(WeaponClass, SpawnParams);
	if (!Weapon)
	{
		UE_LOG(LogTemp, Error, TEXT("%s: failed to spawn %s"), *GetName(), *WeaponClass->GetName());
		return nullptr;
	}

	Weapon->AttachToComponent(GetMesh(),
		FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("WeaponSocket"));

	// Observer wiring lives here too now
	Weapon->OnAmmoChanged.AddDynamic(this, &ACharacterCodeTest::HandleWeaponAmmoChanged);

	return Weapon;
}
