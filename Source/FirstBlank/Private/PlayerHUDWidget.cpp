// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUDWidget.h"
#include "CharacterCodeTest.h"
#include "AWeapon.h"
#include "ArenaGameState.h" 
#include "Components/ProgressBar.h"

void UPlayerHUDWidget::UpdateHealth(float CurrentHealth, float MaxHealth)
{
	HealthPercent = MaxHealth > 0.0f ? FMath::Clamp(CurrentHealth / MaxHealth, 0.0f, 1.0f) : 0.0f;
}

void UPlayerHUDWidget::UpdateStamina(float CurrentStamina, float MaxStamina)
{
	StaminaPercent = MaxStamina > 0.0f ? FMath::Clamp(CurrentStamina / MaxStamina, 0.0f, 1.0f) : 0.0f;
}

void UPlayerHUDWidget::UpdateAmmo(const FString& NewAmmoText)
{
	if (AmmoText)
	{
		AmmoText->SetText(FText::FromString(NewAmmoText));
	}
}

void UPlayerHUDWidget::UpdateWeaponIcon(UTexture2D* NewIcon)
{
	if (WeaponIconImage && NewIcon)
	{
		WeaponIconImage->SetBrushFromTexture(NewIcon);
	}
}

void UPlayerHUDWidget::BindToCharacter(ACharacterCodeTest* Character)
{
	// Unsubscribe from any previous character first
	if (ACharacterCodeTest* Old = BoundCharacter.Get())
	{
		Old->OnHealthChanged.RemoveDynamic(this, &UPlayerHUDWidget::UpdateHealth);
		Old->OnStaminaChanged.RemoveDynamic(this, &UPlayerHUDWidget::UpdateStamina);
		Old->OnActiveWeaponChanged.RemoveDynamic(this, &UPlayerHUDWidget::HandleWeaponChanged);
		Old->OnActiveAmmoChanged.RemoveDynamic(this, &UPlayerHUDWidget::HandleAmmoChanged);
	}

	BoundCharacter = Character;
	if (!Character) return;
	
	// Subscribe to character's observer stuff
	Character->OnHealthChanged.AddDynamic(this, &UPlayerHUDWidget::UpdateHealth);
	Character->OnStaminaChanged.AddDynamic(this, &UPlayerHUDWidget::UpdateStamina);
	Character->OnActiveWeaponChanged.AddDynamic(this, &UPlayerHUDWidget::HandleWeaponChanged);
	Character->OnActiveAmmoChanged.AddDynamic(this, &UPlayerHUDWidget::HandleAmmoChanged);

	// Catch up on the current state. In case other events fire first.
	UpdateHealth(Character->GetCurrentHealth(), Character->GetMaxHealth());
	UpdateStamina(Character->GetCurrentStamina(), Character->GetMaxStamina());
	HandleWeaponChanged(Character->GetActiveWeapon());
}

void UPlayerHUDWidget::HandleWeaponChanged(AAWeapon* Weapon)
{
	if (!Weapon) return;
	UpdateWeaponIcon(Weapon->GetWeaponIcon());
	UpdateAmmo(Weapon->GetAmmoDisplayText());
}


void UPlayerHUDWidget::HandleAmmoChanged(AAWeapon* Weapon)
{
	if (Weapon) UpdateAmmo(Weapon->GetAmmoDisplayText());
}

// Yes Rico... Kaboom.
void UPlayerHUDWidget::NativeDestruct()
{
	BindToCharacter(nullptr);
	BindToGameState(nullptr);
	Super::NativeDestruct();
}

void UPlayerHUDWidget::BindToGameState(AArenaGameState* GameState)
{
	if (AArenaGameState* Old = BoundGameState.Get())
	{
		Old->OnMatchTimeChanged.RemoveDynamic(this, &UPlayerHUDWidget::HandleMatchTimeChanged);
		Old->OnMatchEnded.RemoveDynamic(this, &UPlayerHUDWidget::HandleMatchEnded);
	}

	BoundGameState = GameState;
	if (!GameState) return;

	GameState->OnMatchTimeChanged.AddDynamic(this, &UPlayerHUDWidget::HandleMatchTimeChanged);
	GameState->OnMatchEnded.AddDynamic(this, &UPlayerHUDWidget::HandleMatchEnded);

	// Catch up on the current state
	HandleMatchTimeChanged(GameState->GetSecondsRemaining(), GameState->GetTimeFraction());
}

void UPlayerHUDWidget::HandleMatchTimeChanged(int32 SecondsRemaining, float Fraction)
{
	if (TimerText)
	{
		TimerText->SetText(FText::FromString(
			FString::Printf(TEXT("%d:%02d"), SecondsRemaining / 60, SecondsRemaining % 60)));

		// Last 30 seconds turn red
		TimerText->SetColorAndOpacity(FSlateColor(
			SecondsRemaining <= 30 ? FLinearColor::Red : FLinearColor::White));
	}
	if (TimerBar)
	{
		TimerBar->SetPercent(Fraction);
	}
}

void UPlayerHUDWidget::HandleMatchEnded()
{
	if (TimerText) TimerText->SetText(FText::FromString(TEXT("0:00")));
}