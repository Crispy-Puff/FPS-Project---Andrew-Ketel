#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LeaderboardWidget.generated.h"

class UTextBlock;
class AArenaGameState;
class UVerticalBox;

UCLASS()
class ULeaderboardWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* LeaderboardRows;

	UPROPERTY(EditDefaultsOnly, Category = "Leaderboard")
	FLinearColor PlayerColour = FLinearColor::Yellow;

	UPROPERTY(EditDefaultsOnly, Category = "Leaderboard")
	FLinearColor OtherColour = FLinearColor::White;

	UPROPERTY(EditDefaultsOnly, Category = "Leaderboard")
	int32 FontSize = 22;

	UFUNCTION()
	void Refresh();

private:
	TWeakObjectPtr<AArenaGameState> BoundState;
	
	UTextBlock* AddRow(const FString& Text, const FLinearColor& Color);
	
};