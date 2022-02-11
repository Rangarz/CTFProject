// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CTF_Base.generated.h"

UCLASS()
class CTFTASK_API ACTF_Base : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACTF_Base();

	class UStaticMeshComponent * BaseMesh;

	UPROPERTY()
	bool IsTeamA;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Materials)
	UMaterial* RedBaseMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Materials)
	UMaterial* BlueBaseMaterial;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
