// Fill out your copyright notice in the Description page of Project Settings.


#include "CTF_Base.h"

// Sets default values
ACTF_Base::ACTF_Base()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACTF_Base::BeginPlay()
{
	Super::BeginPlay();

	if (IsTeamA)
	{
		if (RedBaseMaterial != nullptr)
		{
			BaseMesh->SetMaterial(0, RedBaseMaterial);
		}
	}
	else
	{
		if (BlueBaseMaterial != nullptr)
		{
			BaseMesh->SetMaterial(0, BlueBaseMaterial);
		}
	}
	
}

// Called every frame
void ACTF_Base::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

