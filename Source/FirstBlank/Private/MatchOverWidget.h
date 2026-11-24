#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MatchOverWidget.generated.h"

class UButton;
class UTextBlock;

UCLASS()
class UMatchOverWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget)) UTextBlock* WinnerText;
	UPROPERTY(meta = (BindWidget)) UButton* RestartButton;
	UPROPERTY(meta = (BindWidget)) UButton* QuitButton;

	UPROPERTY(EditDefaultsOnly, Category = "Menu")
	FName MenuLevelName = TEXT("L_MainMenu");

	UFUNCTION() void OnRestartClicked();
	UFUNCTION() void OnQuitClicked();

private:
	void ShowResult();
};