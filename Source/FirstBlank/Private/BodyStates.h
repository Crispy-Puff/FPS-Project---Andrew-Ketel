#pragma once

#include "CoreMinimal.h"
#include "StateMachine.h"

class ACharacterCodeTest;

enum class EBodyState : uint8 { Normal, Sprinting, Crouching, Dead };

// Body states also react to requests, not just ticks
class FBodyState : public TState<ACharacterCodeTest>
{
public:
	virtual void OnSprintSet(ACharacterCodeTest& Ctx, bool bWantSprint) {}
	virtual void OnCrouchToggle(ACharacterCodeTest& Ctx) {}
	virtual void OnJump(ACharacterCodeTest& Ctx) {}
};

// all needs to be one line
#define DECLARE_BODY_STATE(ClassName, DisplayName, bHasEnter, bHasExit)                         \
class ClassName : public FBodyState                                                            \
{                                                                                              \
public:                                                                                        \
virtual const TCHAR* GetName() const override { return TEXT(DisplayName); }                \
virtual void Enter(ACharacterCodeTest& Ctx) override;                                      \
virtual void Exit(ACharacterCodeTest& Ctx) override;                                       \
virtual void Tick(ACharacterCodeTest& Ctx, float DeltaTime) override;                      \
virtual void OnSprintSet(ACharacterCodeTest& Ctx, bool bWantSprint) override;              \
virtual void OnCrouchToggle(ACharacterCodeTest& Ctx) override;                             \
virtual void OnJump(ACharacterCodeTest& Ctx) override;                                     \
};

DECLARE_BODY_STATE(FBodyNormalState,    "Normal",    true, false)
DECLARE_BODY_STATE(FBodySprintingState, "Sprinting", true, false)
DECLARE_BODY_STATE(FBodyCrouchingState, "Crouching", true, true)
DECLARE_BODY_STATE(FBodyDeadState,      "Dead",      true, false)