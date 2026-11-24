#include "BodyStates.h"
#include "CharacterCodeTest.h"

// ---- Normal ----
void FBodyNormalState::Enter(ACharacterCodeTest& Ctx) { Ctx.ApplyRunning(false); }
void FBodyNormalState::Exit(ACharacterCodeTest& Ctx) {}
void FBodyNormalState::Tick(ACharacterCodeTest& Ctx, float DeltaTime) { Ctx.RegenStamina(); }

void FBodyNormalState::OnSprintSet(ACharacterCodeTest& Ctx, bool bWantSprint)
{
	if (bWantSprint && Ctx.CanSprint()) Ctx.SetBodyState(EBodyState::Sprinting);
}
void FBodyNormalState::OnCrouchToggle(ACharacterCodeTest& Ctx) { Ctx.SetBodyState(EBodyState::Crouching); }
void FBodyNormalState::OnJump(ACharacterCodeTest& Ctx)         { Ctx.TryJump(); }

// ---- Sprinting ----
void FBodySprintingState::Enter(ACharacterCodeTest& Ctx) { Ctx.ApplyRunning(true); }
void FBodySprintingState::Exit(ACharacterCodeTest& Ctx) {}

void FBodySprintingState::Tick(ACharacterCodeTest& Ctx, float DeltaTime)
{
	Ctx.DrainStamina();
	if (Ctx.IsStaminaDepleted())
	{
		Ctx.SetExhausted(true);
		Ctx.SetBodyState(EBodyState::Normal);
	}
}

void FBodySprintingState::OnSprintSet(ACharacterCodeTest& Ctx, bool bWantSprint)
{
	if (!bWantSprint) Ctx.SetBodyState(EBodyState::Normal);
}
void FBodySprintingState::OnCrouchToggle(ACharacterCodeTest& Ctx) { Ctx.SetBodyState(EBodyState::Crouching); }
void FBodySprintingState::OnJump(ACharacterCodeTest& Ctx)         { Ctx.TryJump(); }

// ---- Crouching ----

// Crouching doesn't currently work, but isn't required. 
// May review at a later date.

void FBodyCrouchingState::Enter(ACharacterCodeTest& Ctx)
{
	Ctx.ApplyRunning(false);
	Ctx.StartCrouch();
}
void FBodyCrouchingState::Exit(ACharacterCodeTest& Ctx) { Ctx.StopCrouch(); }
void FBodyCrouchingState::Tick(ACharacterCodeTest& Ctx, float DeltaTime) { Ctx.RegenStamina(); }

void FBodyCrouchingState::OnSprintSet(ACharacterCodeTest& Ctx, bool bWantSprint)
{
	if (bWantSprint && Ctx.CanSprint()) Ctx.SetBodyState(EBodyState::Sprinting); 
}
void FBodyCrouchingState::OnCrouchToggle(ACharacterCodeTest& Ctx) { Ctx.SetBodyState(EBodyState::Normal); }
void FBodyCrouchingState::OnJump(ACharacterCodeTest& Ctx)
{
	if (!Ctx.CanJump()) return; 
	Ctx.SetBodyState(EBodyState::Normal);
	Ctx.TryJump();
}

// ---- Dead ----
void FBodyDeadState::Enter(ACharacterCodeTest& Ctx)
{
	Ctx.ApplyRunning(false);
	Ctx.StopCrouch();
	Ctx.ApplyDeathEffects();
}
void FBodyDeadState::Exit(ACharacterCodeTest& Ctx) {}
void FBodyDeadState::Tick(ACharacterCodeTest& Ctx, float DeltaTime) {}
void FBodyDeadState::OnSprintSet(ACharacterCodeTest& Ctx, bool bWantSprint) {}
void FBodyDeadState::OnCrouchToggle(ACharacterCodeTest& Ctx) {}
void FBodyDeadState::OnJump(ACharacterCodeTest& Ctx) {}