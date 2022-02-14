// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Runtime/UMG/Public/UMG.h"
#include "Screen_Widget.generated.h"

/**
 * 
 */
UCLASS()
class CTFTASK_API UScreen_Widget : public UUserWidget
{
	GENERATED_BODY()

private:
	bool bUIIsOn;

public:
	UScreen_Widget(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* EndScreen;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UImage* FlagImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UProgressBar* HealthBar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* TimeBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* ScoreAText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* ScoreBText;

	UFUNCTION()
	void ShowUI(bool IsTrue);

	UFUNCTION()
	void ShowFlag(bool IsTrue);

	UFUNCTION()
	void UpdateHealth(float Health);

	UFUNCTION()
	void ChangeTime(int Time);
	
	UFUNCTION()
	void ChangeScoreA(int Score);

	UFUNCTION()
	void ChangeScoreB(int Score);
};
