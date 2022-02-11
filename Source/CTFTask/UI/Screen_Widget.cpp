// Fill out your copyright notice in the Description page of Project Settings.


#include "Screen_Widget.h"


UScreen_Widget::UScreen_Widget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bUIIsOn = true;
}


void UScreen_Widget::ShowUI(bool IsTrue)
{
	bUIIsOn = IsTrue;

	if(EndScreen != nullptr)
	{
		EndScreen->SetVisibility(IsTrue ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
	if(HealthBar != nullptr)
	{
		HealthBar->SetVisibility(IsTrue ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
	if(FlagImage != nullptr)
	{
		FlagImage->SetVisibility(IsTrue ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}

void UScreen_Widget::ShowFlag(bool IsTrue)
{
	if (FlagImage != nullptr)
	{
		if (bUIIsOn)
		{
			FlagImage->SetVisibility(IsTrue ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
		}
		else
		{
			FlagImage->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UScreen_Widget::UpdateHealth(float Health)
{
	if (HealthBar != nullptr)
	{
		HealthBar->SetPercent(Health);
	}
}