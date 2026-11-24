#include "MainMenuWidget.h"
#include "Components/Button.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"


void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (StartButton) StartButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnStartClicked);
	if (QuitButton)  QuitButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnQuitClicked);
}

void UMainMenuWidget::OnStartClicked()
{
	if (APlayerController* PC = GetOwningPlayer())
	{
		PC->SetInputMode(FInputModeGameOnly());
		PC->bShowMouseCursor = false;
	}
	RemoveFromParent();
	UGameplayStatics::OpenLevel(this, GameLevelName);
}

void UMainMenuWidget::OnQuitClicked()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}