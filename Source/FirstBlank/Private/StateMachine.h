#pragma once

#include "CoreMinimal.h"

template <typename TContext>
class TState
{
public:
	virtual ~TState() = default;

	virtual const TCHAR* GetName() const = 0;

	virtual void Enter(TContext& Ctx) {}
	virtual void Tick(TContext& Ctx, float DeltaTime) {}
	virtual void Exit(TContext& Ctx) {}

	// Used by priority selection (mainly bots, players ignore this)
	virtual bool CanEnter(const TContext& Ctx) const { return false; }
	virtual bool CanContinue(const TContext& Ctx) const { return true; }
};



template <typename TContext>
class TStateMachine
{
public:
	using FState = TState<TContext>;

	// Registration order IS priority order
	void Add(TUniquePtr<FState> State) { States.Add(MoveTemp(State)); }

	FState* GetCurrent() const { return Current; }

	// Exit the old state, then enter the new one
	void ChangeState(TContext& Ctx, FState* Next)
	{
		if (Next == Current) return;
		if (Current) Current->Exit(Ctx);
		Current = Next;
		if (Current) Current->Enter(Ctx);
	}
	
	void Tick(TContext& Ctx, float DeltaTime)
	{
		if (Current) Current->Tick(Ctx, DeltaTime);
	}

	// Option B: pick the highest-priority state that wants to run, then tick it.
	void UpdateByPriority(TContext& Ctx, float DeltaTime)
	{
		FState* Chosen = nullptr;
		for (const TUniquePtr<FState>& S : States)
		{
			FState* Candidate = S.Get();
			const bool bWantsToRun = (Candidate == Current)
				? Candidate->CanContinue(Ctx)
				: Candidate->CanEnter(Ctx);
			if (bWantsToRun)
			{
				Chosen = Candidate;
				break;
			}
		}

		ChangeState(Ctx, Chosen);
		if (Current) Current->Tick(Ctx, DeltaTime);
	}

private:
	TArray<TUniquePtr<FState>> States;
	FState* Current = nullptr;
};

