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

	//Base Logic
	void PlayerBaseInteract(AActor* Player, AActor* Base);

	//Game Flow Handling

	void PlayersInitializeLocation();
	void PlayerDeathHandling(class ACTFTaskCharacter* Player);
	void PlayerRespawnHandling(class ACTFTaskCharacter* Player);
	void GameOverHandling();
};
