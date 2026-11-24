#include "LeaderboardWidget.h"
#include "ArenaGameState.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Blueprint/WidgetTree.h"
#include "GameFramework/Pawn.h"

void ULeaderboardWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (AArenaGameState* GS = GetWorld() ? GetWorld()->GetGameState<AArenaGameState>() : nullptr)
	{
		BoundState = GS;
		GS->OnScoresChanged.AddDynamic(this, &ULeaderboardWidget::Refresh);
	}
	Refresh();
}

void ULeaderboardWidget::NativeDestruct()
{
	if (AArenaGameState* GS = BoundState.Get())
	{
		GS->OnScoresChanged.RemoveDynamic(this, &ULeaderboardWidget::Refresh);
	}
	Super::NativeDestruct();
}

void ULeaderboardWidget::Refresh()
{
	if (!LeaderboardRows) return;
	LeaderboardRows->ClearChildren();
	// Reworked to include colour for player highlight
	AddRow(TEXT("LEADERBOARD"), OtherColour);

	const AArenaGameState* GS = BoundState.Get();
	if (!GS) return;

	int32 Rank = 1;
	for (const FCombatantScore& S : GS->GetSortedScores())
	{
		const APawn* Pawn = Cast<APawn>(S.Actor.Get());
		const bool bIsPlayer = Pawn && Pawn->IsPlayerControlled();

		AddRow(FString::Printf(TEXT("%s%d. %s    K: %d   D: %d"),
			bIsPlayer ? TEXT("> ") : TEXT(""), Rank++, *S.Name, S.Kills, S.Deaths),
			bIsPlayer ? PlayerColour : OtherColour);
	}
}
// adds a new entry to the leaderboard for each player and bot
UTextBlock* ULeaderboardWidget::AddRow(const FString& Text, const FLinearColor& Color)
{
	UTextBlock* Row = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	Row->SetText(FText::FromString(Text));
	Row->SetColorAndOpacity(FSlateColor(Color));

	FSlateFontInfo Font = Row->GetFont();
	Font.Size = FontSize;
	Row->SetFont(Font);

	LeaderboardRows->AddChildToVerticalBox(Row);
	return Row;
}