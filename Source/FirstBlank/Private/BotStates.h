#pragma once

#include "CoreMinimal.h"

#include "StateMachine.h"

enum class EPickupGoal : uint8; // full definition lives in BotAIController.h

class ABotAIController;
using FBotState = TState<ABotAIController>;

// dead
class FBotDeadState : public FBotState
{
public:
	virtual const TCHAR* GetName() const override { return TEXT("Dead"); }
	virtual bool CanEnter(const ABotAIController& Ctx) const override;
	virtual bool CanContinue(const ABotAIController& Ctx) const override;
	virtual void Enter(ABotAIController& Ctx) override;
	virtual void Tick(ABotAIController& Ctx, float DeltaTime) override;
};

// flee
class FBotFleeState : public FBotState
{
public:
	virtual const TCHAR* GetName() const override { return TEXT("Flee"); }
	virtual bool CanEnter(const ABotAIController& Ctx) const override;
	virtual bool CanContinue(const ABotAIController& Ctx) const override;
	virtual void Enter(ABotAIController& Ctx) override;
	virtual void Tick(ABotAIController& Ctx, float DeltaTime) override;
	virtual void Exit(ABotAIController& Ctx) override;
};

// Shared behaviour for pickups
class FBotSeekPickupState : public FBotState
{
public:
	virtual bool CanEnter(const ABotAIController& Ctx) const override;
	virtual bool CanContinue(const ABotAIController& Ctx) const override;
	virtual void Enter(ABotAIController& Ctx) override;
	virtual void Tick(ABotAIController& Ctx, float DeltaTime) override;
	virtual void Exit(ABotAIController& Ctx) override;

protected:
	explicit FBotSeekPickupState(EPickupGoal InKind) : Kind(InKind) {}
	EPickupGoal Kind;
};

// health before combat
class FBotSeekHealthState : public FBotSeekPickupState
{
public:
	FBotSeekHealthState();
	virtual const TCHAR* GetName() const override { return TEXT("SeekHealth"); }
};

// combat
class FBotCombatState : public FBotState
{
public:
	virtual const TCHAR* GetName() const override { return TEXT("Combat"); }
	virtual bool CanEnter(const ABotAIController& Ctx) const override;
	virtual bool CanContinue(const ABotAIController& Ctx) const override;
	virtual void Tick(ABotAIController& Ctx, float DeltaTime) override;
};

// rifle
class FBotSeekRifleState : public FBotSeekPickupState
{
public:
	FBotSeekRifleState();
	virtual const TCHAR* GetName() const override { return TEXT("SeekRifle"); }
};

// patrol
class FBotPatrolState : public FBotState
{
public:
	virtual const TCHAR* GetName() const override { return TEXT("Patrol"); }
	virtual bool CanEnter(const ABotAIController& Ctx) const override { return true; } // fallback
	virtual void Enter(ABotAIController& Ctx) override;
	virtual void Tick(ABotAIController& Ctx, float DeltaTime) override;
	virtual void Exit(ABotAIController& Ctx) override;
};

