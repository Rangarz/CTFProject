// Fill out your copyright notice in the Description page of Project Settings.


#include "TaskGameModeGameplay.h"
#include "TaskHUD.h"
#include "Kismet/KismetSystemLibrary.h"

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

	//Assign Teams

	//Start Timer

}