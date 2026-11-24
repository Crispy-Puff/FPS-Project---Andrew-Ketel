#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

#include "PlayerHUDWidget.generated.h"


class ACharacterCodeTest;
class AAWeapon;

class AArenaGameState;
class UProgressBar;

// Class is mainly just a data bridge from HUD to data and vice versa

UCLASS()
class UPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Pass nullptr to unsubscribe
	void BindToCharacter(ACharacterCodeTest* Character);
	
	
	// Called by the character/controller whenever health changes
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateHealth(float CurrentHealth, float MaxHealth);

	// Called by the character/controller whenever stamina changes
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateStamina(float CurrentStamina, float MaxStamina);

	// Called when ammo changes
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateAmmo(const FString& NewAmmoText);
	
	// Called when weapon changes
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateWeaponIcon(UTexture2D* NewIcon);

	void BindToGameState(AArenaGameState* GameState);
	
protected:
	// Used for health bar
	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	float HealthPercent = 1.0f;
	
	// Used for stamina bar
	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	float StaminaPercent = 1.0f;
	
	// BindWidget requires a Text Block in the Widget Blueprint named exactly "AmmoText"
	UPROPERTY(meta = (BindWidget))
	UTextBlock* AmmoText;
	
	UPROPERTY(meta = (BindWidget))
	UImage* WeaponIconImage;
	
	// New broadcast stuff
	virtual void NativeDestruct() override;

	UFUNCTION()
	void HandleWeaponChanged(AAWeapon* Weapon);   // icon + ammo text

	UFUNCTION()
	void HandleAmmoChanged(AAWeapon* Weapon);     // ammo text only
	
	// Match timer stuff
	UFUNCTION()
	void HandleMatchTimeChanged(int32 SecondsRemaining, float Fraction);

	UFUNCTION()
	void HandleMatchEnded();

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TimerText;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* TimerBar;
	
private:
	TWeakObjectPtr<ACharacterCodeTest> BoundCharacter;
	
	TWeakObjectPtr<AArenaGameState> BoundGameState;
};