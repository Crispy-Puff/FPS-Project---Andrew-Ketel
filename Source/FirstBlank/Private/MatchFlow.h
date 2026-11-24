#pragma once

#include "CoreMinimal.h"

class APlayerController;

namespace MatchFlow
{
	// Unpauses, restores game input, then opens the level
	void LeaveMatch(APlayerController* PC, FName LevelName);
}