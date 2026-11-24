#include "ArenaGameState.h"

void AArenaGameState::InitMatch(int32 DurationSeconds)
{
	MatchDurationSeconds = FMath::Max(1, DurationSeconds);
	SecondsRemaining = MatchDurationSeconds;
	bMatchOver = false;
	OnMatchTimeChanged.Broadcast(SecondsRemaining, GetTimeFraction());
}

void AArenaGameState::SetSecondsRemaining(int32 Seconds)
{
	SecondsRemaining = FMath::Max(0, Seconds);
	OnMatchTimeChanged.Broadcast(SecondsRemaining, GetTimeFraction());
}

void AArenaGameState::EndMatch()
{
	if (bMatchOver) return;
	bMatchOver = true;
	OnMatchEnded.Broadcast();
}

float AArenaGameState::GetTimeFraction() const
{
	return static_cast<float>(SecondsRemaining) / static_cast<float>(MatchDurationSeconds);
}

FCombatantScore* AArenaGameState::FindScore(const AActor* Actor)
{
	return Scores.FindByPredicate([Actor](const FCombatantScore& S) { return S.Actor.Get() == Actor; });
}

void AArenaGameState::RegisterCombatant(AActor* Actor, const FString& BaseName)
{
	if (!Actor || FindScore(Actor)) return; // already registered

	// Number duplicates: "Bot", "Bot 2", "Bot 3"
	int32 SameName = 0;
	for (const FCombatantScore& S : Scores)
	{
		if (S.Name == BaseName || S.Name.StartsWith(BaseName + TEXT(" "))) ++SameName;
	}

	FCombatantScore NewScore;
	NewScore.Actor = Actor;
	NewScore.Name = SameName == 0 ? BaseName : FString::Printf(TEXT("%s %d"), *BaseName, SameName + 1);
	Scores.Add(NewScore);
	OnScoresChanged.Broadcast();
}

void AArenaGameState::RecordDeath(AActor* Victim, AActor* Killer)
{
	if (bMatchOver) return; // the result is frozen once time is up

	if (FCombatantScore* V = FindScore(Victim)) V->Deaths++;
	if (Killer && Killer != Victim)
	{
		if (FCombatantScore* K = FindScore(Killer)) K->Kills++;
	}
	OnScoresChanged.Broadcast();
}

TArray<FCombatantScore> AArenaGameState::GetSortedScores() const
{
	TArray<FCombatantScore> Sorted = Scores;
	Sorted.Sort([](const FCombatantScore& A, const FCombatantScore& B)
	{
		return A.Kills != B.Kills ? A.Kills > B.Kills : A.Deaths < B.Deaths;
	});
	return Sorted;
}