#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "ArenaGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMatchTimeChanged, int32, SecondsRemaining, float, Fraction);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMatchEnded);

struct FCombatantScore
{
	TWeakObjectPtr<AActor> Actor;
	FString Name;
	int32 Kills = 0;
	int32 Deaths = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnScoresChanged);

UCLASS()
class AArenaGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Match")
	FOnMatchTimeChanged OnMatchTimeChanged;
	
	//Match end
	UPROPERTY(BlueprintAssignable, Category = "Match")
	FOnMatchEnded OnMatchEnded;

	void InitMatch(int32 DurationSeconds);
	void SetSecondsRemaining(int32 Seconds);
	void EndMatch();

	int32 GetSecondsRemaining() const { return SecondsRemaining; }
	float GetTimeFraction() const;
	bool IsMatchOver() const { return bMatchOver; }

	//Score changes
	UPROPERTY(BlueprintAssignable, Category = "Match")
	FOnScoresChanged OnScoresChanged;

	void RegisterCombatant(AActor* Actor, const FString& BaseName);
	void RecordDeath(AActor* Victim, AActor* Killer);
	TArray<FCombatantScore> GetSortedScores() const;
	
private:
	int32 MatchDurationSeconds = 300;
	int32 SecondsRemaining = 300;
	bool bMatchOver = false;
	
	FCombatantScore* FindScore(const AActor* Actor);
	TArray<FCombatantScore> Scores;
};