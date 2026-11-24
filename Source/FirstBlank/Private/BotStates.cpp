#include "BotStates.h"
#include "BotAIController.h"
#include "CharacterCodeTest.h"

// ---- Combat ----
bool FBotCombatState::CanEnter(const ABotAIController& Ctx) const
{
	return Ctx.HasTarget();
}

bool FBotCombatState::CanContinue(const ABotAIController& Ctx) const
{
	return Ctx.HasTarget();
}

void FBotCombatState::Tick(ABotAIController& Ctx, float DeltaTime)
{
	if (ACharacterCodeTest* Me = Cast<ACharacterCodeTest>(Ctx.GetPawn()))
	{
		Ctx.CombatStep(Me);
	}
}

// ---- Seek pickup ----
FBotSeekHealthState::FBotSeekHealthState() : FBotSeekPickupState(EPickupGoal::Health) {}
FBotSeekRifleState::FBotSeekRifleState()   : FBotSeekPickupState(EPickupGoal::Rifle) {}

bool FBotSeekPickupState::CanEnter(const ABotAIController& Ctx) const
{
	return Ctx.CanStartGoal(Kind);
}

bool FBotSeekPickupState::CanContinue(const ABotAIController& Ctx) const
{
	return Ctx.CanKeepGoal(Kind);
}

void FBotSeekPickupState::Enter(ABotAIController& Ctx)
{
	Ctx.BeginGoal(Kind);
}

void FBotSeekPickupState::Tick(ABotAIController& Ctx, float DeltaTime)
{
	Ctx.WalkToGoal();

	// Shoot on the move. SeekRifle never has a target, so this only affects SeekHealth
	if (Ctx.HasTarget())
	{
		if (ACharacterCodeTest* Me = Cast<ACharacterCodeTest>(Ctx.GetPawn()))
		{
			Ctx.CombatStep(Me);
		}
	}
}

void FBotSeekPickupState::Exit(ABotAIController& Ctx)
{
	Ctx.EndGoal();
}

// ---- Patrol ----
void FBotPatrolState::Enter(ABotAIController& Ctx)
{
	Ctx.SchedulePatrolIfIdle();
}

void FBotPatrolState::Tick(ABotAIController& Ctx, float DeltaTime)
{
	// Restarts patrolling after a respawn or a finished pickup walk
	Ctx.SchedulePatrolIfIdle();
}

void FBotPatrolState::Exit(ABotAIController& Ctx)
{
	// Only cancel the timer. Do NOT call StopMovement here. Bad things happen
	Ctx.CancelPatrolTimer();
}

// ---- Dead ----
void FBotDeadState::Enter(ABotAIController& Ctx)
{
	Ctx.HandleDeathCleanup();
}

void FBotDeadState::Tick(ABotAIController& Ctx, float DeltaTime)
{
	// Just in case
	Ctx.CancelPatrolTimer();
}

bool FBotDeadState::CanEnter(const ABotAIController& Ctx) const
{
	return Ctx.IsPawnDead();
}

bool FBotDeadState::CanContinue(const ABotAIController& Ctx) const
{
	return Ctx.IsPawnDead();
}

// ---- Flee ----
bool FBotFleeState::CanEnter(const ABotAIController& Ctx) const    { return Ctx.ShouldFlee(); }
bool FBotFleeState::CanContinue(const ABotAIController& Ctx) const { return Ctx.ShouldFlee(); }
void FBotFleeState::Enter(ABotAIController& Ctx)                   { Ctx.BeginFlee(); }
void FBotFleeState::Tick(ABotAIController& Ctx, float DeltaTime)   { Ctx.FleeTick(); }
void FBotFleeState::Exit(ABotAIController& Ctx)                    { Ctx.EndFlee(); }

