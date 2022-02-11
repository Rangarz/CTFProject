// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "CTF_GameState.generated.h"

/**
 * 
 */
UCLASS()
class CTFTASK_API ACTF_GameState : public AGameState
{
	GENERATED_BODY()
	

public:
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	int TeamAScore;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	int TeamBScore;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	int Time;

public:
	
	ACTF_GameState();


	void AssignTeamStates();

	void StartTimer();


private:
	void PassTime();
};
