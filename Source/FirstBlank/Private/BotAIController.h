// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "StateMachine.h" 
#include "BotStates.h"


#include "BotAIController.generated.h"


class ACharacterCodeTest;

enum class EPickupGoal : uint8 { None, Health, Rifle };

UCLASS()
class ABotAIController : public AAIController
{
	GENERATED_BODY()
	
	
public:
	ABotAIController();
	
	// The character the bot is currently engaging (null while patrolling)
	AActor* GetCurrentTarget() const { return CurrentTarget.Get(); }
	bool IsFleeing() const { return bFleeing; }
	
	// State design content
	bool HasTargetOrGoal() const { return CurrentTarget.IsValid() || PickupGoal.IsValid(); }
	
	// Differentiates
	bool HasTarget() const { return CurrentTarget.IsValid(); }
	bool HasPickupGoal() const { return PickupGoal.IsValid(); }
	
	void SchedulePatrolIfIdle();
	void CancelPatrolTimer();
	void CombatStep(ACharacterCodeTest* Me);   // move this declaration here from private

	const TCHAR* GetBrainStateName() const
	{
		return Brain.GetCurrent() ? Brain.GetCurrent()->GetName() : TEXT("None");
	}
	
	bool IsPawnDead() const;
	void HandleDeathCleanup();
	
	bool CanStartGoal(EPickupGoal Kind) const;
	bool CanKeepGoal(EPickupGoal Kind) const;
	void BeginGoal(EPickupGoal Kind);
	void WalkToGoal();
	void EndGoal();
	
	// new flee details
	bool ShouldFlee() const;
	void BeginFlee();
	void FleeTick();
	void EndFlee();
	
protected:
	
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;
    
	// How far from its current position the bot picks its next destination
	UPROPERTY(EditDefaultsOnly, Category = "Bot | Patrol")
	float PatrolRadius = 2000.0f;
    
	// Pause between arriving and choosing the next point
	UPROPERTY(EditDefaultsOnly, Category = "Bot | Patrol")
	float PatrolWaitTime = 1.5f;
	
	// Enhanced behaviors
	UPROPERTY(EditDefaultsOnly, Category = "Bot | Sight")
	float SightRadius = 2500.0f;

	// Must be larger than SightRadius, so targets don't flicker at the edge
	UPROPERTY(EditDefaultsOnly, Category = "Bot | Sight")
	float LoseSightRadius = 3000.0f;	// default 3000.0f

	// Half-angle applied here (I.E. 90 here = 180 angle vision)
	UPROPERTY(EditDefaultsOnly, Category = "Bot | Sight")
	float PeripheralVisionAngle = 90.0f;
	
	// Range to shoot
	UPROPERTY(EditDefaultsOnly, Category = "Bot | Combat")
	float FireRange = 2500.0f; 
	
	// Pickup logic below
	UPROPERTY(EditDefaultsOnly, Category = "Bot | Pickups")
	float PickupSightRange = 2500.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Bot | Pickups")
	int32 HealthSeekThreshold = 75; // seek health at or below this

	UPROPERTY(EditDefaultsOnly, Category = "Bot | Pickups")
	float RifleSeekAmmoFraction = 0.5f; // seek a rifle below this total-ammo fraction
	
	UPROPERTY(EditDefaultsOnly, Category = "Bot | Flee")
	int32 FleeHealthThreshold = 25;

	UPROPERTY(EditDefaultsOnly, Category = "Bot | Flee")
	float FleeDistance = 1500.0f;	// default 1500.0f

	UPROPERTY(EditDefaultsOnly, Category = "Bot | Flee")
	float FleeRepathInterval = 1.5f;	// default 1.5f
	
	UPROPERTY(EditDefaultsOnly, Category = "Bot | Flee")
	float FleeMemoryTime = 4.0f; // keep fleeing this long after losing sight

	// Must be a UFUNCTION so it can be bound to the perception delegate
	UFUNCTION()
	void OnTargetPerceived(AActor* Actor, FAIStimulus Stimulus);

	
	static constexpr float DecisionInterval = 0.15f;
	TStateMachine<ABotAIController> Brain;
    
private:
	// basic movement and targeting
	void MoveToRandomPoint();
	void UpdateTarget();
	
	FTimerHandle PatrolTimer;
	TArray<TWeakObjectPtr<AActor>> VisibleTargets;
	TWeakObjectPtr<AActor> CurrentTarget;
	
	void SetFacingTarget(bool bFaceTarget);
	
	// Decision methods
	void DecisionTick();

	bool NeedsHealth(const ACharacterCodeTest* Me) const;
	bool NeedsRifle(const ACharacterCodeTest* Me) const;
	bool IsGoalStillWanted(const ACharacterCodeTest* Me) const;
	void SetPickupGoal(AActor* Goal, EPickupGoal Kind);
	void ClearPickupGoal();
		
	FTimerHandle DecisionTimer;
	TWeakObjectPtr<AActor> PickupGoal;
	EPickupGoal GoalKind = EPickupGoal::None;
	
	// flee details (mostly moved to public for states)
	bool FindFleePoint(const FVector& From, const FVector& ThreatLoc, FVector& OutPoint) const;
	
	float FleeBlockedUntil = 0.0f;

	bool bFleeing = false;
	float LastFleeRepathTime = -100.0f;
	
	TWeakObjectPtr<AActor> LastThreat;
	float FleeUntilTime = 0.0f;
};
