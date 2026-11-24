#include "MenuPlayerController.h"
#include "Blueprint/UserWidget.h"

void AMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!MenuWidgetClass) return;

	MenuWidget = CreateWidget<UUserWidget>(this, MenuWidgetClass);
	if (MenuWidget)
	{
		MenuWidget->AddToViewport();

		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(MenuWidget->TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		SetInputMode(InputMode);

		bShowMouseCursor = true;
	}
}