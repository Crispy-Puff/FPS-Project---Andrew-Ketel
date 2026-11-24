#include "MatchFlow.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

void MatchFlow::LeaveMatch(APlayerController* PC, FName LevelName)
{
	if (!PC) return;

	PC->SetPause(false); // otherwise the new level can start frozen
	PC->SetInputMode(FInputModeGameOnly());
	PC->bShowMouseCursor = false;
	UGameplayStatics::OpenLevel(PC, LevelName);
}