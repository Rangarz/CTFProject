// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TaskGameMode.h"
#include "TaskGameModeGameplay.generated.h"

/**
 * 
 */
UCLASS()
class CTFTASK_API ATaskGameModeGameplay : public ATaskGameMode
{
	GENERATED_BODY()

	//UPROPERTY()
	int ReadyInstancesNum;

private:
	void StartCaptureTheFlag();

	void TerminateSessionCheck();

public:
	ATaskGameModeGameplay();

	void InstanceReady();

	//Flag Logic
	void PlayerFlagInteract(AActor* Player, AActor* Flag);

	void PlayerDeathHandling(AActor* Player);

	//Base Logic
	void PlayerBaseInteract(AActor* Player, AActor* Base);

	//Game Over Handling
	void GameOverHandling();
};
