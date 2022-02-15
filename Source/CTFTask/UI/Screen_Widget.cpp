// Fill out your copyright notice in the Description page of Project Settings.


#include "Screen_Widget.h"

#include <string>


UScreen_Widget::UScreen_Widget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bUIIsOn = true;
}


void UScreen_Widget::ShowUI(bool IsTrue)
{
	bUIIsOn = IsTrue;
	
	if(HealthBar != nullptr)
	{
		HealthBar->SetVisibility(IsTrue ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
		HealthImage->SetVisibility(IsTrue ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
	
	if(FlagImage != nullptr)
	{
		if(IsTrue == false)
		{
			FlagImage->SetVisibility(ESlateVisibility::Hidden);
		}
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

void UScreen_Widget::ChangeTime(int Time)
{
	if(TimeBox != nullptr)
	{
		TimeBox->SetText(FText::FromString(FString::FromInt(Time)));
	}
}

void UScreen_Widget::ChangeScoreA(int Score)
{
	if(ScoreAText != nullptr)
	{
		ScoreAText->SetText(FText::FromString(FString::FromInt(Score)));
	}
}

void UScreen_Widget::ChangeScoreB(int Score)
{
	if(ScoreBText != nullptr)
	{
		ScoreBText->SetText(FText::FromString(FString::FromInt(Score)));
	}
}

void UScreen_Widget::EnableEndScreen(FText text)
{
	EndScreen->SetVisibility(ESlateVisibility::Visible);
	EndScreen->SetText(text);
}
