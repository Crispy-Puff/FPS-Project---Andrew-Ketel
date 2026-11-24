#include "MatchOverWidget.h"
#include "ArenaGameState.h"
#include "MatchFlow.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"

void UMatchOverWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (RestartButton) RestartButton->OnClicked.AddDynamic(this, &UMatchOverWidget::OnRestartClicked);
	if (QuitButton)    QuitButton->OnClicked.AddDynamic(this, &UMatchOverWidget::OnQuitClicked);
	ShowResult();
}

void UMatchOverWidget::ShowResult()
{
	if (!WinnerText) return;

	const AArenaGameState* GS = GetWorld() ? GetWorld()->GetGameState<AArenaGameState>() : nullptr;
	const TArray<FCombatantScore> Sorted = GS ? GS->GetSortedScores() : TArray<FCombatantScore>();
	if (Sorted.Num() == 0)
	{
		WinnerText->SetText(FText::FromString(TEXT("MATCH OVER")));
		return;
	}

	const FCombatantScore& Top = Sorted[0];
	const bool bDraw = Sorted.Num() > 1
		&& Sorted[1].Kills == Top.Kills && Sorted[1].Deaths == Top.Deaths;

	const APawn* TopPawn = Cast<APawn>(Top.Actor.Get());
	const bool bPlayerWon = TopPawn && TopPawn->IsPlayerControlled();

	FString Text;
	if (bDraw)           Text = TEXT("DRAW");
	else if (bPlayerWon) Text = TEXT("YOU WIN!");
	else                 Text = FString::Printf(TEXT("%s WINS"), *Top.Name);

	WinnerText->SetText(FText::FromString(Text));
}

void UMatchOverWidget::OnRestartClicked()
{
	MatchFlow::LeaveMatch(GetOwningPlayer(),
		FName(*UGameplayStatics::GetCurrentLevelName(this, true)));
}

void UMatchOverWidget::OnQuitClicked()
{
	MatchFlow::LeaveMatch(GetOwningPlayer(), MenuLevelName);
}