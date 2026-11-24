#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputComponent.h"
#include "CharacterCodeTest.h"
#include "PlayerHUDWidget.h"
#include <EnhancedInputSubsystems.h>
#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"


UCLASS(Abstract)
class AMyPlayerController : public APlayerController
{
public:
	virtual void BeginPlay() override;
	
	// | symbol allows you to create / group categories
	
	// Input action to map the movement
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Input | Character Movement")
	UInputAction* ActionMove = nullptr;
	
	// Input Action to map the look around feature
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Input | Character Movement")
	UInputAction* ActionLookAround = nullptr;
	
	// Input Action to map the jump feature
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Input | Character Movement")
	UInputAction* ActionJump = nullptr;
	
	// Crouch
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Input | Character Movement")
	UInputAction* ActionCrouch = nullptr;

	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Input | Character Movement")
	TObjectPtr<UInputAction>ActionToggleSprint = nullptr;
	
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Input | Character Movement")
	TObjectPtr<UInputMappingContext>InputMappingContent = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Input | Combat")
	TObjectPtr<UInputAction>ActionFire = nullptr;
	
	// Reload
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Input | Combat")
	UInputAction* ActionReload = nullptr;
	
	// weapon swaps
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Input | Combat")
	UInputAction* ActionSwitchToPistol = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Input | Combat")
	UInputAction* ActionSwitchToRifle = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player | UI")
	TSubclassOf<UPlayerHUDWidget> PlayerHUDClass;
	
	UPROPERTY()
	TObjectPtr<UPlayerHUDWidget> PlayerHUDWidget;

	void HandleLookAround(const FInputActionValue& InputActionValue);
	void HandleMove(const FInputActionValue& InputActionValue);
	void HandleJump();
	void HandleCrouch();
	void HandleToggleSprint();
	
	void HandleFire();
	void HandleReload();
	
	void HandleSwitchToPistol();
	void HandleSwitchToRifle();
	
	void AccessScreenshot(int Width, int Height, const TArray<FColor>& Colors);
	
	virtual void OnPossess(APawn* InPawn) override;
	
	virtual void OnUnPossess() override;

	// Pause stuff
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Input | UI")
	UInputAction* ActionPause = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Player | UI")
	TSubclassOf<UUserWidget> PauseMenuClass;

	void HandlePause();
	void ResumeGame();
	
	// game end
	UPROPERTY(EditDefaultsOnly, Category = "Player | UI")
	TSubclassOf<UUserWidget> MatchOverClass;
	
private:
	UPROPERTY()
	UEnhancedInputComponent* EnhancedInputComponent = nullptr;
	
	UPROPERTY()
	ACharacterCodeTest* PlayerCharacter = nullptr;
	
	void OpenPauseMenu();
	UPROPERTY()
	TObjectPtr<UUserWidget> PauseMenuWidget;
	
	UFUNCTION()
	void HandleMatchEnded();
	
	UPROPERTY()
	TObjectPtr<UUserWidget> MatchOverWidget;
	
	GENERATED_BODY()
};
