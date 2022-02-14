// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "CTF_GameState.generated.h"

DECLARE_MULTICAST_DELEGATE(FCSOnTimeChanged);
DECLARE_MULTICAST_DELEGATE(FCSOnScoreAChanged);
DECLARE_MULTICAST_DELEGATE(FCSOnScoreBChanged);

/**
 * 
 */
UCLASS()
class CTFTASK_API ACTF_GameState : public AGameState
{
	GENERATED_BODY()
	

public:
	
	UPROPERTY(ReplicatedUsing=OnScoreAChanged, VisibleAnywhere, BlueprintReadOnly)
	int TeamAScore;

	UPROPERTY(ReplicatedUsing=OnScoreBChanged, VisibleAnywhere, BlueprintReadOnly)
	int TeamBScore;

	UPROPERTY(ReplicatedUsing=OnTimeChanged, VisibleAnywhere, BlueprintReadOnly)
	int Time;

public:
	ACTF_GameState();


	void AssignTeamStates();

	void StartTimer();


	FCSOnTimeChanged OnTimeChangedEvent;
	FCSOnScoreAChanged OnScoreAChangedEvent;
	FCSOnScoreAChanged OnScoreBChangedEvent;
	
private:
	UFUNCTION()
	void PassTime();

	UFUNCTION()
	void OnTimeChanged();

	UFUNCTION()
	void OnScoreAChanged();

	UFUNCTION()
	void OnScoreBChanged();
};
