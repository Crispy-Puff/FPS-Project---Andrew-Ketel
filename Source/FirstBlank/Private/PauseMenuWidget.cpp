#include "PauseMenuWidget.h"
#include "MyPlayerController.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "MatchFlow.h"

void UPauseMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ResumeButton)  ResumeButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnResumeClicked);
	if (RestartButton) RestartButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnRestartClicked);
	if (QuitButton)    QuitButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnQuitClicked);
}

void UPauseMenuWidget::OnResumeClicked()
{
	if (AMyPlayerController* PC = Cast<AMyPlayerController>(GetOwningPlayer()))
	{
		PC->ResumeGame();
	}
}

void UPauseMenuWidget::OnRestartClicked()
{
	LeaveMatch(FName(*UGameplayStatics::GetCurrentLevelName(this, true)));
}

void UPauseMenuWidget::OnQuitClicked()
{
	LeaveMatch(MenuLevelName);
}

// calls to MatchFlow instead
void UPauseMenuWidget::LeaveMatch(FName LevelName)
{
	MatchFlow::LeaveMatch(GetOwningPlayer(), LevelName);
}