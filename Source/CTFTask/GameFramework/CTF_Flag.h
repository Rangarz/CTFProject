// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CTF_Flag.generated.h"

UCLASS()
class CTFTASK_API ACTF_Flag : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACTF_Flag();

	class UStaticMeshComponent * FlagMesh;

	UPROPERTY()
	FTransform InitialTransform;

	UPROPERTY()
	bool IsTeamA;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Materials)
	UMaterial* RedFlagMaterial;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Materials)
	UMaterial* BlueFlagMaterial;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastReturnFlag();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastEnableFlag(bool IsEnabled);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastMoveToLocation(FVector Location);
};
