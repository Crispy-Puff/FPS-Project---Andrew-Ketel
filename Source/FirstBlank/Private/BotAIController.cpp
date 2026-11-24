// Fill out your copyright notice in the Description page of Project Settings.


#include "BotAIController.h"
#include "CharacterCodeTest.h"
#include "Perception/AISenseConfig_Sight.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NavigationSystem.h"
#include "EngineUtils.h"
#include "RiflePickup.h"
#include "HealthPickup.h"

namespace
{
	template <typename T>
	T* FindClosestVisiblePickup(const AAIController* Ctrl, float Range)
	{
		const APawn* Pawn = Ctrl->GetPawn();
		if (!Pawn) return nullptr;

		T* Best = nullptr;
		float BestDistSq = Range * Range;

		for (TActorIterator<T> It(Pawn->GetWorld()); It; ++It)
		{
			T* Pickup = *It;
			if (!Pickup->IsAvailable()) continue;

			const float DistSq = FVector::DistSquared(Pawn->GetActorLocation(), Pickup->GetActorLocation());
			if (DistSq >= BestDistSq) continue;
			if (!Ctrl->LineOfSightTo(Pickup)) continue;

			Best = Pickup;
			BestDistSq = DistSq;
		}
		return Best;
	}
}

ABotAIController::ABotAIController()
{
	// Values here relating to UProperties won't change in editor.
	// They are changed on creation here.
	
	// The controller owns the perception component
	SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception")));
	
	// Sight configuration
	UAISenseConfig_Sight* SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = SightRadius;
	SightConfig->LoseSightRadius = LoseSightRadius;
	SightConfig->PeripheralVisionAngleDegrees = PeripheralVisionAngle;
	SightConfig->SetMaxAge(5.0f); // forget a target 5s after losing sight

	// Neutral as default.
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

	GetPerceptionComponent()->ConfigureSense(*SightConfig);
	GetPerceptionComponent()->SetDominantSense(SightConfig->GetSenseImplementation());
	
	// States registration
	Brain.Add(MakeUnique<FBotDeadState>());
	Brain.Add(MakeUnique<FBotFleeState>());
	Brain.Add(MakeUnique<FBotSeekHealthState>());
	Brain.Add(MakeUnique<FBotCombatState>());
	Brain.Add(MakeUnique<FBotSeekRifleState>());
	Brain.Add(MakeUnique<FBotPatrolState>());
}

void ABotAIController::BeginPlay()
{
	Super::BeginPlay();
	GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &ABotAIController::OnTargetPerceived);
}


void ABotAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// Short delay so the navmesh is ready before the first move
	GetWorldTimerManager().SetTimer(PatrolTimer, this, &ABotAIController::MoveToRandomPoint, 0.5f, false);
	
	// Decision gap
	GetWorldTimerManager().SetTimer(DecisionTimer, this, &ABotAIController::DecisionTick, 0.15f, true);
}

void ABotAIController::MoveToRandomPoint()
{
	if (CurrentTarget.IsValid() || PickupGoal.IsValid() || bFleeing) return; // Stop patrolling if we've got something to do
	if (const ACharacterCodeTest* Me = Cast<ACharacterCodeTest>(GetPawn()); !Me || Me->IsDead()) return; // If dead do nothing
	
	APawn* BotPawn = GetPawn();
	if (!BotPawn) return;

	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	FNavLocation Destination;

	if (NavSys && NavSys->GetRandomReachablePointInRadius(BotPawn->GetActorLocation(), PatrolRadius, Destination))
	{
		MoveToLocation(Destination.Location, 50.0f);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Bot: no navmesh point found, retrying"));
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &ABotAIController::MoveToRandomPoint, PatrolWaitTime, false);
	}
}

void ABotAIController::OnTargetPerceived(AActor* Actor, FAIStimulus Stimulus)
{
	// Ignore non-characters.
	if (!Cast<ACharacterCodeTest>(Actor)) return;

	if (Stimulus.WasSuccessfullySensed())
	{
		VisibleTargets.AddUnique(Actor);
	}
	else
	{
		VisibleTargets.Remove(Actor);
	}

	UpdateTarget();
}

void ABotAIController::UpdateTarget()
{
	VisibleTargets.RemoveAll([](const TWeakObjectPtr<AActor>& A) { return !A.IsValid(); });

	APawn* BotPawn = GetPawn();
	if (!BotPawn) return;

	// Pick the closest visible character
	AActor* Best = nullptr;
	float BestDistSq = TNumericLimits<float>::Max();
	for (const TWeakObjectPtr<AActor>& Candidate : VisibleTargets)
	{
		const ACharacterCodeTest* C = Cast<ACharacterCodeTest>(Candidate.Get());
		if (!C || C->IsDead()) continue;
		
		const float DistSq = FVector::DistSquared(BotPawn->GetActorLocation(), Candidate->GetActorLocation());
		if (DistSq < BestDistSq)
		{
			BestDistSq = DistSq;
			Best = Candidate.Get();
		}
	}

	const bool bHadTarget = CurrentTarget.IsValid();
	CurrentTarget = Best; // set BEFORE StopMovement, see OnMoveCompleted below

	if (Best)
	{
		LastThreat = Best;
		if (!bFleeing)
		{
			SetFocus(Best);
			if (!bHadTarget)
			{
				SetFacingTarget(true);
				GetWorldTimerManager().ClearTimer(PatrolTimer);
				StopMovement();
			}
		}
	}
	else if (bHadTarget)
	{
		ClearFocus(EAIFocusPriority::Gameplay);
		SetFacingTarget(false);
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &ABotAIController::MoveToRandomPoint, PatrolWaitTime, false);
	}
}

void ABotAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);
	
	if (CurrentTarget.IsValid() || PickupGoal.IsValid() || bFleeing) return;

	GetWorldTimerManager().SetTimer(PatrolTimer, this, &ABotAIController::MoveToRandomPoint, PatrolWaitTime, false);
}

void ABotAIController::SetFacingTarget(bool bFaceTarget)
{
	if (ACharacter* Char = Cast<ACharacter>(GetPawn()))
	{
		UCharacterMovementComponent* Move = Char->GetCharacterMovement();
		Move->bOrientRotationToMovement = !bFaceTarget;   // patrol: face travel direction
		Move->bUseControllerDesiredRotation = bFaceTarget; // combat: face the focus
	}
	
}

void ABotAIController::DecisionTick()
{
	ACharacterCodeTest* Me = Cast<ACharacterCodeTest>(GetPawn());
	if (!Me) return;

	// the Dead state runs
	if (!Me->IsDead())
	{
		UpdateTarget();
	}
	Brain.UpdateByPriority(*this, DecisionInterval);
}

void ABotAIController::CombatStep(ACharacterCodeTest* Me)
{
	AActor* Target = CurrentTarget.Get();
	if (!Target) return;

	Me->SelectBestWeapon();

	if (Me->NeedsReload())
	{
		Me->Reload();
		return;
	}

	if (FVector::Dist(Me->GetActorLocation(), Target->GetActorLocation()) <= FireRange)
	{
		Me->Fire();
	}
}

bool ABotAIController::NeedsHealth(const ACharacterCodeTest* Me) const
{
	return Me->GetCurrentHealth() <= HealthSeekThreshold;
}

bool ABotAIController::NeedsRifle(const ACharacterCodeTest* Me) const
{
	return Me->GetRifleTotalAmmoFraction() < RifleSeekAmmoFraction;
}

bool ABotAIController::IsGoalStillWanted(const ACharacterCodeTest* Me) const
{
	if (!PickupGoal.IsValid()) return false;

	switch (GoalKind)
	{
	case EPickupGoal::Health:
		{
			const AHealthPickup* P = Cast<AHealthPickup>(PickupGoal.Get());
			return P && P->IsAvailable() && NeedsHealth(Me);
		}
	case EPickupGoal::Rifle:
		{
			const ARiflePickup* P = Cast<ARiflePickup>(PickupGoal.Get());
			// "unless it is shooting something": abandon the rifle if a target appears
			return P && P->IsAvailable() && NeedsRifle(Me) && !CurrentTarget.IsValid();
		}
	default:
		return false;
	}
}

void ABotAIController::SetPickupGoal(AActor* Goal, EPickupGoal Kind)
{
	PickupGoal = Goal;
	GoalKind = Kind;
	GetWorldTimerManager().ClearTimer(PatrolTimer);
	MoveToActor(Goal, 20.0f);
}

void ABotAIController::ClearPickupGoal()
{
	PickupGoal = nullptr;
	GoalKind = EPickupGoal::None;
}



// Fleeing methods
bool ABotAIController::FindFleePoint(const FVector& From, const FVector& ThreatLoc, FVector& OutPoint) const
{
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	// testing guard
	if (!NavSys) return false;

	const FVector Away = (From - ThreatLoc).GetSafeNormal2D();
	float BestDist = FVector::Dist2D(From, ThreatLoc) + 300.0f; // [Default +300]
	bool bFound = false; 

	for (int32 i = 0; i < 8; ++i)
	{
		// Sample directions within 70 degrees of "straight away" and keep the best
		const FVector Dir = Away.RotateAngleAxis(FMath::FRandRange(-70.0f, 70.0f), FVector::UpVector);

		FNavLocation Projected;
		if (!NavSys->ProjectPointToNavigation(From + Dir * FleeDistance, Projected, FVector(500.f, 500.f, 300.f)))
		{
			continue; // that direction is off the navmesh (wall, void)
		}

		const float Dist = FVector::Dist2D(Projected.Location, ThreatLoc);
		if (Dist > BestDist)
		{
			BestDist = Dist;
			OutPoint = Projected.Location;
			bFound = true;
		}
	}
	return bFound;
}

bool ABotAIController::ShouldFlee() const
{
	const ACharacterCodeTest* Me = Cast<ACharacterCodeTest>(GetPawn());
	if (!Me || Me->IsDead()) return false;

	const float Now = GetWorld()->GetTimeSeconds();
	if (Now < FleeBlockedUntil) return false; // cornered recently, fight instead
	if (Me->GetCurrentHealth() > FleeHealthThreshold) return false;

	return CurrentTarget.IsValid() || Now < FleeUntilTime;
}

void ABotAIController::BeginFlee()
{
	bFleeing = true;
	LastFleeRepathTime = -100.0f; // pick a destination on the first tick

	ClearPickupGoal();
	CancelPatrolTimer();
	ClearFocus(EAIFocusPriority::Gameplay); // face travel direction
	SetFacingTarget(false);

	if (ACharacterCodeTest* Me = Cast<ACharacterCodeTest>(GetPawn()))
	{
		Me->SetRunning(true);
	}
}

void ABotAIController::FleeTick()
{
	const float Now = GetWorld()->GetTimeSeconds();

	AActor* Threat = CurrentTarget.IsValid() ? CurrentTarget.Get() : LastThreat.Get();
	if (!Threat)
	{
		FleeBlockedUntil = Now + 0.5f;
		return;
	}
	if (CurrentTarget.IsValid()) FleeUntilTime = Now + FleeMemoryTime;

	const bool bNeedsNewPoint = GetMoveStatus() == EPathFollowingStatus::Idle
		|| Now - LastFleeRepathTime > FleeRepathInterval;
	if (!bNeedsNewPoint) return;

	FVector Destination;
	if (!FindFleePoint(GetPawn()->GetActorLocation(), Threat->GetActorLocation(), Destination))
	{
		FleeBlockedUntil = Now + 0.5f; // cornered: CanContinue turns false and combat runs
		return;
	}

	LastFleeRepathTime = Now;
	MoveToLocation(Destination, 50.0f);
}

void ABotAIController::EndFlee()
{
	bFleeing = false;

	if (ACharacterCodeTest* Me = Cast<ACharacterCodeTest>(GetPawn()))
	{
		Me->SetRunning(false);
	}
	if (AActor* Target = CurrentTarget.Get())
	{
		// Cornered or healed with the enemy still in view: its go time baby
		SetFocus(Target);
		SetFacingTarget(true);
	}
}


// here be dragons below (summative code) (In retrospect the code above is also summative related)
// Patrol
void ABotAIController::SchedulePatrolIfIdle()
{
	if (GetMoveStatus() == EPathFollowingStatus::Idle
		&& !GetWorldTimerManager().IsTimerActive(PatrolTimer))
	{
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &ABotAIController::MoveToRandomPoint, PatrolWaitTime, false);
	}
}

void ABotAIController::CancelPatrolTimer()
{
	GetWorldTimerManager().ClearTimer(PatrolTimer);
}

// Death
bool ABotAIController::IsPawnDead() const
{
	const ACharacterCodeTest* Me = Cast<ACharacterCodeTest>(GetPawn());
	return Me && Me->IsDead();
}


void ABotAIController::HandleDeathCleanup()
{
	CurrentTarget = nullptr;   
	
	ClearPickupGoal();
	ClearFocus(EAIFocusPriority::Gameplay);
	SetFacingTarget(false);
	StopMovement();
	CancelPatrolTimer();       
	
	FleeUntilTime = 0.0f;
	FleeBlockedUntil = 0.0f;
	
}

// Goals
bool ABotAIController::CanStartGoal(EPickupGoal Kind) const
{
	const ACharacterCodeTest* Me = Cast<ACharacterCodeTest>(GetPawn());
	if (!Me) return false;

	if (Kind == EPickupGoal::Health)
	{
		return NeedsHealth(Me) && FindClosestVisiblePickup<AHealthPickup>(this, PickupSightRange);
	}
	if (Kind == EPickupGoal::Rifle)
	{
		// "unless shooting something"
		return !CurrentTarget.IsValid() && NeedsRifle(Me)
			&& FindClosestVisiblePickup<ARiflePickup>(this, PickupSightRange);
	}
	return false;
}

bool ABotAIController::CanKeepGoal(EPickupGoal Kind) const
{
	// Nothing new to do
	const ACharacterCodeTest* Me = Cast<ACharacterCodeTest>(GetPawn());
	return Me && GoalKind == Kind && IsGoalStillWanted(Me);
}

void ABotAIController::BeginGoal(EPickupGoal Kind)
{
	// Look for something new to do
	AActor* Goal = nullptr;
	if (Kind == EPickupGoal::Health)     Goal = FindClosestVisiblePickup<AHealthPickup>(this, PickupSightRange);
	else if (Kind == EPickupGoal::Rifle) Goal = FindClosestVisiblePickup<ARiflePickup>(this, PickupSightRange);

	if (Goal) SetPickupGoal(Goal, Kind);
}

void ABotAIController::WalkToGoal()
{
	// Does what it says on the tin
	if (PickupGoal.IsValid() && GetMoveStatus() == EPathFollowingStatus::Idle)
	{
		MoveToActor(PickupGoal.Get(), 20.0f);
	}
}

void ABotAIController::EndGoal()
{
	ClearPickupGoal(); 
}