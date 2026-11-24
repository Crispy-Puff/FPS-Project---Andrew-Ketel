// Fill out your copyright notice in the Description page of Project Settings.


#include "BotCharacter.h"
#include "BotAIController.h"		
#include "DrawDebugHelpers.h"
#include "BotAIController.h"
#include "GameFramework/CharacterMovementComponent.h"

ABotCharacter::ABotCharacter()
{
	// Which controller drives this pawn, and when it gets one
	AIControllerClass = ABotAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// Face the direction of travel (players use the camera for this instead)
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
	
	DisplayName = TEXT("Bot");
}

void ABotCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	const ABotAIController* BotAI = Cast<ABotAIController>(GetController());
	const AActor* Target = BotAI ? BotAI->GetCurrentTarget() : nullptr;
	
	
	// Skip debug if dead
	if (IsDead()) return;
	
	// China's great wall of debug text
	/*
	const FString Info = FString::Printf(TEXT("HP %d/%d | Stamina %.0f\n%s\n%s"),
	GetCurrentHealth(), GetMaxHealth(), GetCurrentStamina(),
	*GetActiveWeaponAmmoText(),
	BotAI && BotAI->IsFleeing() ? TEXT("FLEEING")
	: (Target ? *FString::Printf(TEXT("Target: %s"), *Target->GetName()) : TEXT("Patrolling")));

	DrawDebugString(GetWorld(), FVector(0.f, 0.f, 120.f), Info, this,
		FColor::White, 0.1f);
	*/
}

