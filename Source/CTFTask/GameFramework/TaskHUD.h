// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Components/WidgetComponent.h"
#include "CTFTask/UI/Screen_Widget.h"
#include "TaskHUD.generated.h"

class UTexture2D;

UCLASS()
class ATaskHUD : public AHUD
{
	GENERATED_BODY()

private:

	UPROPERTY()
	UTexture2D* CrosshairTex = nullptr;

	UPROPERTY()
	bool bShowCursor = false;

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TSoftObjectPtr<UTexture2D> CrosshairTexture;


public:
	ATaskHUD();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

	UFUNCTION()
	void ShowScreenUI(bool IsTrue);

	UFUNCTION()
	void ShowScreenFlag(bool IsTrue);

	UFUNCTION()
	void ShowCursor(bool IsTrue);

	UFUNCTION()
	void UpdateHealth(float Health);

	UPROPERTY(EditDefaultsOnly, Category = "Widgets");
	TSubclassOf<UUserWidget> ScreenWidgetClass;


private:
	UScreen_Widget* ScreenWidget;

	class ACTF_GameState* GameState;

	UFUNCTION()
	void OnTimeChanged();

	UFUNCTION()
	void OnScoreAChanged();

	UFUNCTION()
	void OnScoreBChanged();
};

