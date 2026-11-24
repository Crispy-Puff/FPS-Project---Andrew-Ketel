// Fill out your copyright notice in the Description page of Project Settings.
#include "MyPlayerController.h"
#include "ArenaGameState.h"
#include <EnhancedInputDeveloperSettings.h>
#include "GameFramework/CharacterMovementComponent.h"

#include "Engine/LocalPlayer.h"

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Don't inherit the menu's UI-only input mode or cursor
	SetInputMode(FInputModeGameOnly());
	bShowMouseCursor = false;
}

void AMyPlayerController::HandleLookAround(const FInputActionValue& InputActionValue)
{
	const FVector2D LookAxisVector = InputActionValue.Get<FVector2D>();
	
	AddYawInput((LookAxisVector.X));
	AddPitchInput((LookAxisVector.Y));
}



void AMyPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	PlayerCharacter = Cast<ACharacterCodeTest>(InPawn);
	
	// debug
	checkf(PlayerCharacter, TEXT("APlayerControllerB derived classes should only posses ACharacterCodeTest derived pawns"));
	
	EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	
	// debug
	checkf(EnhancedInputComponent, TEXT("Unable to get reference to the EnhancedInputComponent"));
	
	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	
	// debug
	checkf(InputSubsystem, TEXT("Unable to get reference to the EnhancedLocalPlayerSubsystem"));
	
	// debug
	checkf(InputMappingContent, TEXT("Input mapping was not specified"));
	
	InputSubsystem->ClearAllMappings();
	InputSubsystem->AddMappingContext(InputMappingContent, 0);
	
	if (ActionMove)
	{
		EnhancedInputComponent->BindAction(ActionMove, ETriggerEvent::Triggered, this, &AMyPlayerController::HandleMove);
	}
	
	if (ActionLookAround)
	{
		EnhancedInputComponent->BindAction(ActionLookAround, ETriggerEvent::Triggered, this, &AMyPlayerController::HandleLookAround);
	}
	
	if (ActionJump)
	{
		EnhancedInputComponent->BindAction(ActionJump, ETriggerEvent::Triggered, this, &AMyPlayerController::HandleJump);
	}
	
	if (ActionCrouch)
	{
		EnhancedInputComponent->BindAction(ActionCrouch, ETriggerEvent::Triggered, this, &AMyPlayerController::HandleCrouch);
	}
	
	if (ActionToggleSprint)
	{
		EnhancedInputComponent->BindAction(ActionToggleSprint, ETriggerEvent::Triggered, this, &AMyPlayerController::HandleToggleSprint);
	}
	
	if (ActionFire)
	{
		EnhancedInputComponent->BindAction(ActionFire, ETriggerEvent::Triggered, this, &AMyPlayerController::HandleFire);
	}
	
	if (ActionReload)
	{
		EnhancedInputComponent->BindAction(ActionReload, ETriggerEvent::Started, this, &AMyPlayerController::HandleReload);
	}
	
	if (ActionSwitchToPistol)
	{
		EnhancedInputComponent->BindAction(ActionSwitchToPistol, ETriggerEvent::Started, this, &AMyPlayerController::HandleSwitchToPistol);
	}

	if (ActionSwitchToRifle)
	{
		EnhancedInputComponent->BindAction(ActionSwitchToRifle, ETriggerEvent::Started, this, &AMyPlayerController::HandleSwitchToRifle);
	}
	
	// new pause action
	if (ActionPause)
	{
		EnhancedInputComponent->BindAction(ActionPause, ETriggerEvent::Started, this, &AMyPlayerController::HandlePause);
	}
	
	if (PlayerHUDClass)
	{
		PlayerHUDWidget = CreateWidget<UPlayerHUDWidget>(this, PlayerHUDClass);
		if (PlayerHUDWidget)
		{
			PlayerHUDWidget->AddToViewport();
			
			// Widget and character introduced
			PlayerHUDWidget->BindToCharacter(PlayerCharacter);
			
			// Game state added
			PlayerHUDWidget->BindToGameState(GetWorld()->GetGameState<AArenaGameState>());
			
			if (AArenaGameState* GS = GetWorld()->GetGameState<AArenaGameState>())
			{
				GS->OnMatchEnded.AddUniqueDynamic(this, &AMyPlayerController::HandleMatchEnded);
			}
		}
	}
	
}

void AMyPlayerController::HandleMove(const FInputActionValue& InputActionValue)
{
	const FVector2D MovementVector = InputActionValue.Get<FVector2D>();
	
	if (PlayerCharacter)
	{
		PlayerCharacter->AddMovementInput(PlayerCharacter->GetActorForwardVector(), MovementVector.Y);
		PlayerCharacter->AddMovementInput(PlayerCharacter->GetActorRightVector(), MovementVector.X);
	}
}

void AMyPlayerController::HandleJump()
{
	// state now handles it
	if (PlayerCharacter) PlayerCharacter->Jump();   
}

void AMyPlayerController::HandleCrouch()
{
	// hopefully state handles it better
	if (PlayerCharacter) PlayerCharacter->RequestCrouchToggle();
}

void AMyPlayerController::HandleToggleSprint()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->ToggleRunning();
	}
}

void AMyPlayerController::HandleFire()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->Fire();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerCharacter is NULL in HandleFire"));
	}
}

void AMyPlayerController::HandleReload()
{
	if (PlayerCharacter)
	{
		PlayerCharacter -> Reload();
	}
}


void AMyPlayerController::OnUnPossess()
{
	EnhancedInputComponent->ClearActionBindings();
	
	if (PlayerHUDWidget) 
		PlayerHUDWidget->BindToCharacter(nullptr);
	
	if (AArenaGameState* GS = GetWorld() ? GetWorld()->GetGameState<AArenaGameState>() : nullptr)
	{
		GS->OnMatchEnded.RemoveDynamic(this, &AMyPlayerController::HandleMatchEnded);
	}
	
	Super::OnUnPossess();
}

void AMyPlayerController::HandleSwitchToPistol()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->SwitchToWeaponSlot(1);
	}
}

void AMyPlayerController::HandleSwitchToRifle()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->SwitchToWeaponSlot(2);
	}
}

// new summative stuff (pause related)
void AMyPlayerController::HandlePause()
{
	if (IsPaused()) ResumeGame();
	else OpenPauseMenu();
}

void AMyPlayerController::OpenPauseMenu()
{
	// No pausing once the match is over
	if (const AArenaGameState* GS = GetWorld()->GetGameState<AArenaGameState>())
	{
		if (GS->IsMatchOver()) return;
	}
	if (!PauseMenuClass) return;

	if (!PauseMenuWidget)
	{
		PauseMenuWidget = CreateWidget<UUserWidget>(this, PauseMenuClass);
	}
	if (!PauseMenuWidget) return;

	PauseMenuWidget->AddToViewport(100); // above the HUD
	SetPause(true);

	FInputModeGameAndUI Mode;
	Mode.SetWidgetToFocus(PauseMenuWidget->TakeWidget());
	Mode.SetHideCursorDuringCapture(false);
	SetInputMode(Mode);
	bShowMouseCursor = true;
}

void AMyPlayerController::ResumeGame()
{
	if (PauseMenuWidget) PauseMenuWidget->RemoveFromParent();

	SetPause(false);
	SetInputMode(FInputModeGameOnly());
	bShowMouseCursor = false;
}

void AMyPlayerController::HandleMatchEnded()
{
	// Close the pause menu if open somehow
	if (PauseMenuWidget) PauseMenuWidget->RemoveFromParent();

	if (!MatchOverClass) return;

	MatchOverWidget = CreateWidget<UUserWidget>(this, MatchOverClass);
	if (!MatchOverWidget) return;

	MatchOverWidget->AddToViewport(200); // above HUD and pause menu

	SetPause(true); // freezes bots and respawn timers

	FInputModeUIOnly Mode;
	Mode.SetWidgetToFocus(MatchOverWidget->TakeWidget());
	SetInputMode(Mode);
	bShowMouseCursor = true;
}
