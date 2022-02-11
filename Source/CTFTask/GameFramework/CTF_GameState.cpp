// Fill out your copyright notice in the Description page of Project Settings.


#include "CTF_GameState.h"
#include "Net/UnrealNetwork.h"

ACTF_GameState::ACTF_GameState()
{
	TeamAScore = 0;
	TeamBScore = 0;
	Time = 60;
}

void ACTF_GameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACTF_GameState, TeamAScore);
	DOREPLIFETIME(ACTF_GameState, TeamBScore);
	DOREPLIFETIME(ACTF_GameState, Time);
}