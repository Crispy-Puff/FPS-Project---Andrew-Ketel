#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MenuPlayerController.generated.h"

class UUserWidget;

UCLASS()
class AMenuPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Menu")
	TSubclassOf<UUserWidget> MenuWidgetClass;

private:
	UPROPERTY()
	TObjectPtr<UUserWidget> MenuWidget;
};