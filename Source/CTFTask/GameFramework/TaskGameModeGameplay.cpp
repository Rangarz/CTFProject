// Fill out your copyright notice in the Description page of Project Settings.


#include "TaskGameModeGameplay.h"
#include "TaskHUD.h"
#include "Kismet/KismetSystemLibrary.h"
#include "CTF_GameState.h"
#include "CTF_PlayerState.h"
#include "CTF_GameInstanceOnline.h"

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


		if(Player->FlagHeld != nullptr)
		{
			//Player have a flag
			Player->FlagHeld->EnableFlag(true);
			Player->FlagHeld->MulticastMoveToLocation(Player->GetActorLocation() + FVector(100,100,0));
			Player->SetFlag(nullptr);
			
		}
		else
		{
			//Player doesn't have a flag
			Player->SetFlag(nullptr);
		}
		
		//Respawn player in 3 seconds
		FTimerHandle UnusedHandle;
		FTimerDelegate RespawnDelegate = FTimerDelegate::CreateUObject(this, &ATaskGameModeGameplay::PlayerRespawnHandling, Player);
		GetWorldTimerManager().SetTimer(UnusedHandle, RespawnDelegate, 3.0f, false);

	}
}

void ATaskGameModeGameplay::PlayerRespawnHandling(ACTFTaskCharacter* Player)
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
			Player->SetActorLocation(FVector(-1360, 1110, 200), false, nullptr, ETeleportType::None);
		}
		else
		{
			//Teleport player to Team B's Spawn Point
			Player->SetActorLocation(FVector(1220.0, -1040, 200), false, nullptr, ETeleportType::None);
		}
	}

}

void ATaskGameModeGameplay::PlayerFlagInteract(ACTFTaskCharacter* Player, ACTF_Flag* Flag)
{
	//Get Player and Flag Team
	ACTF_PlayerState* PS = (ACTF_PlayerState* )Player->GetPlayerState();
	if(PS->IsTeamA == Flag->IsTeamA)
	{
		//Same Team, Caught own flag
		Flag->ResetFlag();
	}
	else
	{
		//Opposite Team, Caught enemy flag
		Player->SetFlag(Flag);
		Flag->EnableFlag(false);
	}
	
}

void ATaskGameModeGameplay::PlayerBaseInteract(ACTFTaskCharacter* Player, ACTF_Base* Base)
{
	ACTF_PlayerState* PS = (ACTF_PlayerState* )Player->GetPlayerState();
	if(PS->IsTeamA == Base->IsTeamA)
	{
		//Home base
		if(Player->FlagHeld != nullptr)
		{
			//Player is now trying to score, check if other player doesn't have a flag
			TArray<AActor*> FoundPlayers;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACTFTaskCharacter::StaticClass(), FoundPlayers);

			for (int i = 0; i < FoundPlayers.Num(); i++)
			{
				ACTFTaskCharacter* FoundPlayer = (ACTFTaskCharacter* )FoundPlayers[i];
				if (FoundPlayer != nullptr)
				{
					if(FoundPlayer != Player)
					{
						//We found the enemy player
						//Check if he is holding the flag
						if(FoundPlayer->FlagHeld != nullptr)
						{
							//Player holding a flag can't score
						}
						else
						{
							//Player doesn't have flag, we can score
							Player->FlagHeld->ResetFlag();
							Player->FlagHeld->EnableFlag(true);
							Player->SetFlag(nullptr);

							ACTF_GameState* ACTFGameState = (ACTF_GameState*)GetWorld()->GetGameState();

							if(ACTFGameState != nullptr)
							{
								if(PS->IsTeamA)
								{
									ACTFGameState->AddScoreA();
								}
								else
								{
									ACTFGameState->AddScoreB();
								}
							}
						}
					}
				}
			}
		}
	}
	else
	{
		//Enemy base
	}
}

void ATaskGameModeGameplay::GameOverHandling()
{
	TArray<AActor*> FoundPlayers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACTFTaskCharacter::StaticClass(), FoundPlayers);

	for (int i = 0; i < FoundPlayers.Num(); i++)
	{
		ACTFTaskCharacter* FoundPlayer = (ACTFTaskCharacter* )FoundPlayers[i];
		if (FoundPlayer != nullptr)
		{
			FoundPlayer->GetCharacterMovement()->DisableMovement();
			FoundPlayer->bCanShoot = false;
		}
	}
	
	FTimerHandle UnusedHandle;
	GetWorldTimerManager().SetTimer(UnusedHandle, this, &ATaskGameModeGameplay::CloseGame, 3.0f, false);

}

void ATaskGameModeGameplay::CloseGame()
{
	TArray<AActor*> FoundPlayers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACTFTaskCharacter::StaticClass(), FoundPlayers);

	for (int i = 0; i < FoundPlayers.Num(); i++)
	{
		ACTFTaskCharacter* FoundPlayer = (ACTFTaskCharacter* )FoundPlayers[i];
		if (FoundPlayer != nullptr)
		{
			FoundPlayer->GameEnded();
		}
	}

	FTimerHandle UnusedHandle;
	GetWorldTimerManager().SetTimer(UnusedHandle, this, &ATaskGameModeGameplay::TerminateSession, 1.0f, true);
}

void ATaskGameModeGameplay::TerminateSession()
{
	if(GetNumPlayers() == 1)
	{
		//Destroy session
		UCTF_GameInstanceOnline* GameInstance = (UCTF_GameInstanceOnline* )GetGameInstance();

		if(GameInstance)
		{
			GameInstance->OnDestroySessionCompleteEvent.AddDynamic(this, &ATaskGameModeGameplay::OnSessionEnded);

			GameInstance->DestroySession();
		}
	}
}

void ATaskGameModeGameplay::OnSessionEnded(bool Successful)
{
	if(Successful)
	{
		UGameplayStatics::OpenLevel(GetWorld(), FName("MainMenu"));
	}
}



