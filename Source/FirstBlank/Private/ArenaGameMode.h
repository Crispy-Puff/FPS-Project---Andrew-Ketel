#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ArenaGameMode.generated.h"

class AArenaGameState;

UCLASS()
class AArenaGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AArenaGameMode();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Match")
	int32 MatchLengthSeconds = 300; // 5 minutes

private:
	void TickMatchClock();

	FTimerHandle MatchClockTimer;
	TWeakObjectPtr<AArenaGameState> MatchState;
};