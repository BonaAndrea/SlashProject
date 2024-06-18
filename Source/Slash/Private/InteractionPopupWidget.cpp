// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionPopupWidget.h"
#include "Components/TextBlock.h"

void UInteractionPopupWidget::SetInteractionText(const FText& Text)
{
	if (InteractionTextBlock)
	{
		InteractionTextBlock->SetText(Text);
	}
}
