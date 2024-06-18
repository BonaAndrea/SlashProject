// Copyright Epic Games, Inc. All Rights Reserved.


#include "SlashGameModeBase.h"

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

void ASlashGameModeBase::TogglePause()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		if (IsPaused())
		{
			if (PauseWidget && PauseWidget->IsInViewport())
			{
				PauseWidget->RemoveFromParent();
			}
			PlayerController->SetInputMode(FInputModeGameOnly());
			PlayerController->bShowMouseCursor = false;
			UGameplayStatics::SetGamePaused(GetWorld(), false);
		}
		else
		{
			if (PauseWidgetClass)
			{
				PauseWidget = CreateWidget<UUserWidget>(GetWorld(), PauseWidgetClass);
				if (PauseWidget)
				{
					PauseWidget->AddToViewport();
				}
			}
			PlayerController->SetInputMode(FInputModeUIOnly());
			PlayerController->bShowMouseCursor = true;
			UGameplayStatics::SetGamePaused(GetWorld(), true);
		}
	}
}

void ASlashGameModeBase::GameOver(bool hasWon)
{
	if (GameOverWidgetClass && GameWinWidgetClass)
	{
		GameOverWidget = nullptr;
		switch(hasWon)
		{
		case true:
			GameOverWidget = CreateWidget<UUserWidget>(GetWorld(), GameWinWidgetClass);
			break;
		default:
			GameOverWidget = CreateWidget<UUserWidget>(GetWorld(), GameOverWidgetClass);
		}
		if (GameOverWidget)
		{
			GameOverWidget->AddToViewport();

			APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
			if (PlayerController)
			{
				PlayerController->SetInputMode(FInputModeUIOnly());
				PlayerController->bShowMouseCursor = true;
				PlayerController->DisableInput(nullptr);
			}
		}
	}
}


