#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

class UButton;

UCLASS()
class UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	// Names must match the widgets in the Designer exactly
	UPROPERTY(meta = (BindWidget))
	UButton* StartButton;

	UPROPERTY(meta = (BindWidget))
	UButton* QuitButton;

	// Must match the arena level's asset name
	UPROPERTY(EditDefaultsOnly, Category = "Menu")
	FName GameLevelName = TEXT("NewWorld");

	UFUNCTION()
	void OnStartClicked();

	UFUNCTION()
	void OnQuitClicked();
};