// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SlashGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API ASlashGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere);
	TSubclassOf<UUserWidget> PauseWidgetClass;

	UUserWidget* PauseWidget;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> GameOverWidgetClass;
	UUserWidget* GameOverWidget;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> GameWinWidgetClass;
	UUserWidget* GameWinWidget;
	
	bool IsPauseOpen = false;

public:
	void TogglePause();
	void GameOver(bool hasWon);
};
