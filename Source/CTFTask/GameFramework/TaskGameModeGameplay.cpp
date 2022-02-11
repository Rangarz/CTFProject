// Fill out your copyright notice in the Description page of Project Settings.


#include "TaskGameModeGameplay.h"
#include "TaskHUD.h"
#include "Kismet/KismetSystemLibrary.h"
#include "CTF_GameState.h"
#include "TaskCharacter.h"
#include "CTF_PlayerState.h"

ATaskGameModeGameplay::ATaskGameModeGameplay() 
{
	HUDClass = ATaskHUD::StaticClass();

	ReadyInstancesNum = 0;
}

void ATaskGameModeGameplay::InstanceReady()
{
	ReadyInstancesNum++;

	if (ReadyInstancesNum == 2)
	{
		//Add two seconds delay then start match

		FTimerHandle UnusedHandle;
		GetWorldTimerManager().SetTimer(UnusedHandle, this, &ATaskGameModeGameplay::StartCaptureTheFlag, 2.0f, false);
	}
}

void ATaskGameModeGameplay::StartCaptureTheFlag()
{
	UKismetSystemLibrary::PrintString(this->GetWorld(), "Match Started!", true, true, FColor::Blue, 2.0f);

	//Get Game state
	ACTF_GameState* ACTFGameState = (ACTF_GameState*)GetWorld()->GetGameState();

	if(ACTFGameState != nullptr)
	{
		//Assign Teams
		ACTFGameState->AssignTeamStates();
		//Start Timer
		ACTFGameState->StartTimer();
	}

	//Initialize Player Locations
	PlayersInitializeLocation();
}

void ATaskGameModeGameplay::PlayersInitializeLocation()
{
	TArray<AActor*> FoundPlayers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACTFTaskCharacter::StaticClass(), FoundPlayers);

	for (int i = 0; i < FoundPlayers.Num(); i++)
	{
		ACTFTaskCharacter* Player = (ACTFTaskCharacter* )FoundPlayers[i];
		if (Player != nullptr)
		{
			ACTF_PlayerState* PS = (ACTF_PlayerState*)Player->GetPlayerState();
			if (PS != nullptr)
			{
				if (PS->IsTeamA)
				{
					Player->SetActorLocation(FVector(-1360, 1110, 200));
				}
				else
				{
					Player->SetActorLocation(FVector(1220, -1040, 200));

				}
			}
		}
	}
}

void ATaskGameModeGameplay::PlayerDeathHandling(ACTFTaskCharacter* Player)
{
	if(Player)
	{
		//Stop player from being able to move
		Player->GetCharacterMovement()->DisableMovement();
		
		//Respawn player in 3 seconds
		FTimerHandle UnusedHandle;
		GetWorldTimerManager().SetTimer(UnusedHandle, [&]() { this->PlayerRespawnHandling(Player); }, 3.0f, false);

	}
}

void ATaskGameModeGameplay::PlayerRespawnHandling(class ACTFTaskCharacter* Player)
{
	//Enable Movement
	Player->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	//Adjust health
	Player->AddHealthPoints(100.0f);
	//Turn off ragdoll on server
	Player->RagdollEffect(false);
	//Tell clients to setup client-side respawn
	Player->MulticastRespawnPlayer();
	//Teleport player to the correct position
	ACTF_PlayerState* PS = (ACTF_PlayerState*)Player->GetPlayerState();
	if (PS)
	{
		if (PS->IsTeamA)
		{
			//Teleport player to Team A's Spawn Point
			Player->SetActorLocation(FVector(-1360, 1110, 200), false, nullptr, ETeleportType::TeleportPhysics);
		}
		else
		{
			//Teleport player to Team B's Spawn Point
			Player->SetActorLocation(FVector(1220.0, -1040, 200), false, nullptr, ETeleportType::TeleportPhysics);
		}
	}

}