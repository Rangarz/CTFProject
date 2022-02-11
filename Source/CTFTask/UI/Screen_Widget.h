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

	UFUNCTION()
	void ShowUI(bool IsTrue);

	UFUNCTION()
	void ShowFlag(bool IsTrue);

	UFUNCTION()
	void UpdateHealth(float Health);
};
