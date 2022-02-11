// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "CTF_PlayerState.generated.h"

/**
 * 
 */
UCLASS()
class CTFTASK_API ACTF_PlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	UPROPERTY(ReplicatedUsing = OnTeamChanged, VisibleAnywhere, BlueprintReadOnly)
	bool IsTeamA;


public:

	UFUNCTION()
	void OnTeamChanged();

};
