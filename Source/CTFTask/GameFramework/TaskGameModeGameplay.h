// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TaskGameMode.h"
#include "TaskCharacter.h"
#include "CTF_Flag.h"
#include "CTF_Base.h"
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
	void PlayerFlagInteract(ACTFTaskCharacter* Player, ACTF_Flag* Flag);

	//Base Logic
	void PlayerBaseInteract(ACTFTaskCharacter* Player, ACTF_Base* Base);

	//Game Flow Handling

	void PlayersInitializeLocation();
	void PlayerDeathHandling(ACTFTaskCharacter* Player);
	void PlayerRespawnHandling(ACTFTaskCharacter* Player);
	void GameOverHandling();

	UFUNCTION()
	void CloseGame();

	UFUNCTION()
	void TerminateSession();

	UFUNCTION()
	void OnSessionEnded(bool Successful);
};
