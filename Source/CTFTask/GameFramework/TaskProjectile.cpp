// Copyright Epic Games, Inc. All Rights Reserved.

#include "TaskProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TaskCharacter.h"
#include "Kismet/KismetSystemLibrary.h"

ACTFTaskProjectile::ACTFTaskProjectile() 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &ACTFTaskProjectile::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
}

void ACTFTaskProjectile::BeginPlay()
{
	Super::BeginPlay();


	//Only spawner of this projectile can check for collisions
	if (GetLocalRole() != ROLE_Authority)
	{
		CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}



	//Only spawner of this projectile hears its sound
	if (GetLocalRole() == ROLE_Authority && FireSound != NULL)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}
}

void ACTFTaskProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if(OtherActor->GetClass()->IsChildOf(ACTFTaskCharacter::StaticClass()))
	{
		ACTFTaskCharacter* TaskCharacter = Cast<ACTFTaskCharacter>(OtherActor);

		if (OtherActor != NULL && TaskCharacter != NULL && TaskCharacter != ProjectileShooter)
		{
			//Only projectiles spawned by server and on server will affect health
			if(GetLocalRole() == ROLE_Authority && bFakeProjectile == false)
			{
				UKismetSystemLibrary::PrintString(this->GetWorld(), "Collided with player", true, true, FColor::Blue, 2.0f);

				TaskCharacter->AddHealthPoints(-10.0f);

				Destroy();
			}

			//If fake projectile hits character, must destroy
			if (bFakeProjectile == true)
			{
				TaskCharacter->IsHit();
				Destroy();
			}
		}
	}
	else
	{
		// Only add impulse and destroy projectile if we hit a physics
		if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL))
		{
			//Only a real projectile can affect other actors
			if (OtherComp->IsSimulatingPhysics() && bFakeProjectile == false)
			{
				OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
			}

			if(bFakeProjectile)
			{
				UKismetSystemLibrary::PrintString(this->GetWorld(), "Destroying projectile", true, true, FColor::Blue, 2.0f);
			}

			Destroy();
		}
	}
}