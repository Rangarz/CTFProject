// Fill out your copyright notice in the Description page of Project Settings.


#include "CTF_Flag.h"

// Sets default values
ACTF_Flag::ACTF_Flag()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	FlagMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FlagMesh"));
}

// Called when the game starts or when spawned
void ACTF_Flag::BeginPlay()
{
	Super::BeginPlay();

	InitialTransform = GetActorTransform();

	if (IsTeamA)
	{
		if(RedFlagMaterial != nullptr)
		{
			FlagMesh->SetMaterial(0, RedFlagMaterial);
		}
	}
	else
	{
		if (BlueFlagMaterial != nullptr)
		{
			FlagMesh->SetMaterial(0, BlueFlagMaterial);
		}
	}
}

// Called every frame
void ACTF_Flag::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACTF_Flag::ResetFlag_Implementation()
{
	SetActorTransform(InitialTransform);
}

void ACTF_Flag::EnableFlag_Implementation(bool IsEnabled)
{
	FlagMesh->SetVisibility(IsEnabled);

	FlagMesh->SetCollisionEnabled(IsEnabled? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
}

void ACTF_Flag::MulticastMoveToLocation_Implementation(FVector Location)
{
	SetActorLocation(Location);
}
