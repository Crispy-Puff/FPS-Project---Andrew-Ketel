#include "ArenaGameMode.h"
#include "ArenaGameState.h"
#include "TimerManager.h"

AArenaGameMode::AArenaGameMode()
{
	GameStateClass = AArenaGameState::StaticClass();
}

void AArenaGameMode::BeginPlay()
{
	Super::BeginPlay();

	MatchState = GetGameState<AArenaGameState>();
	if (!MatchState.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("ArenaGameMode: GameState is not AArenaGameState"));
		return;
	}

	MatchState->InitMatch(MatchLengthSeconds);
	GetWorldTimerManager().SetTimer(MatchClockTimer, this, &AArenaGameMode::TickMatchClock, 1.0f, true);
}

void AArenaGameMode::TickMatchClock()
{
	AArenaGameState* State = MatchState.Get();
	if (!State) return;

	State->SetSecondsRemaining(State->GetSecondsRemaining() - 1);

	if (State->GetSecondsRemaining() <= 0)
	{
		GetWorldTimerManager().ClearTimer(MatchClockTimer);
		State->EndMatch(); // the match-over screen will hook in here
	}
}