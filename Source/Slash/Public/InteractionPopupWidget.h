// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractionPopupWidget.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API UInteractionPopupWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Funzione per impostare il testo di interazione
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void SetInteractionText(const FText& Text);

protected:
	// Il puntatore al TextBlock
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* InteractionTextBlock;
	
};
