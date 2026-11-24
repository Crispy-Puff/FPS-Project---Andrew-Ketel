#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseMenuWidget.generated.h"

class UButton;

UCLASS()
class UPauseMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget)) UButton* ResumeButton;
	UPROPERTY(meta = (BindWidget)) UButton* RestartButton;
	UPROPERTY(meta = (BindWidget)) UButton* QuitButton;

	UPROPERTY(EditDefaultsOnly, Category = "Menu")
	FName MenuLevelName = TEXT("L_MainMenu");

	UFUNCTION() void OnResumeClicked();
	UFUNCTION() void OnRestartClicked();
	UFUNCTION() void OnQuitClicked();

private:
	void LeaveMatch(FName LevelName);
};