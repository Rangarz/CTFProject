// Fill out your copyright notice in the Description page of Project Settings.


#include "CTF_GameState.h"
#include "Net/UnrealNetwork.h"
#include "CTF_PlayerState.h"
#include "TaskGameModeGameplay.h"


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
		for (int i = 0; i < PlayerArray.Num(); i++)
		{
			ACTF_PlayerState* PS = (ACTF_PlayerState* )PlayerArray[i];
			PS->IsTeamA = i == 0;
			PS->OnTeamChanged();
		}
	}
	else
	{
		//This is an invalid number to start with
	}
}

void ACTF_GameState::StartTimer()
{
	FTimerHandle UnusedHandle;
	GetWorldTimerManager().SetTimer(UnusedHandle, this, &ACTF_GameState::PassTime, 1.0f, true);
}

void ACTF_GameState::PassTime()
{
	if(Time == 0)
	{
		//Game ended
		ATaskGameModeGameplay* GameMode = (ATaskGameModeGameplay *)GetWorld()->GetAuthGameMode();
		if(GameMode != nullptr)
		{
			GameMode->GameOverHandling();
		}
	}
	else
	{
		Time--;
		OnTimeChanged();
	}
}

void ACTF_GameState::OnTimeChanged()
{
	OnTimeChangedEvent.Broadcast();
}

void ACTF_GameState::OnScoreAChanged()
{
	OnScoreAChangedEvent.Broadcast();
}

void ACTF_GameState::OnScoreBChanged()
{
	OnScoreBChangedEvent.Broadcast();
}

void ACTF_GameState::AddScoreA()
{
	TeamAScore++;
	OnScoreAChanged();
}

void ACTF_GameState::AddScoreB()
{
	TeamBScore++;
	OnScoreBChanged();
}




