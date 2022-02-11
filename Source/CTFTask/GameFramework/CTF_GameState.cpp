// Fill out your copyright notice in the Description page of Project Settings.


#include "CTF_GameState.h"
#include "Net/UnrealNetwork.h"
#include "CTF_PlayerState.h"

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

void ACTF_GameState::AssignTeamStates()
{
	if (PlayerArray.Num() == 2)
	{
		//We can start assigning teams

	}
	else
	{
		//This is an invalid number to start with
		for (int i = 0; i < PlayerArray.Num(); i++)
		{
			ACTF_PlayerState* PS = (ACTF_PlayerState* )PlayerArray[i];
			PS->IsTeamA = i == 0;
		}
	}
}

void ACTF_GameState::StartTimer()
{

}