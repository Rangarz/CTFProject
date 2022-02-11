// Fill out your copyright notice in the Description page of Project Settings.


#include "CTF_PlayerState.h"
#include "Net/UnrealNetwork.h"


void ACTF_PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACTF_PlayerState, IsTeamA);
}


void ACTF_PlayerState::OnTeamChanged()
{
	
}