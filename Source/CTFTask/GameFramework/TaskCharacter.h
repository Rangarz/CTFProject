// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TaskCharacter.generated.h"

class UInputComponent;

UCLASS(config=Game)
class ACTFTaskCharacter : public ACharacter
{
	GENERATED_BODY()


	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* FP_MuzzleLocation;

	/** Gun mesh: VR view (attached to the VR controller directly, no arm, just the actual gun) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* VR_Gun;

	/** Location on VR gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* VR_MuzzleLocation;


	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

	/** Motion controller (right hand) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UMotionControllerComponent* R_MotionController;

	/** Motion controller (left hand) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UMotionControllerComponent* L_MotionController;


public:
	ACTFTaskCharacter();


protected:
	virtual void BeginPlay();

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector GunOffset;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class ACTFTaskProjectile> ProjectileClass;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class ACTFTaskProjectile> FakeProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	class USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* FireAnimation;

	/** AnimMontage to play each time we get hit */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* HitAnimation;

	//Materials for team coloring	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Materials)
	UMaterial* RedBodyMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Materials)
	UMaterial* BlueBodyMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Materials)
	UMaterial* RedFlagMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Materials)
	UMaterial* BlueFlagMaterial;

	/** Whether to use motion controller location for aiming. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	uint32 bUsingMotionControllers : 1;

	/*Third Person Mesh*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Mesh)
	class USkeletalMeshComponent* TP_Body;

	/*Third Person Gun*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Mesh)
	class USkeletalMeshComponent* TP_Gun;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Mesh)
	class USkeletalMeshComponent* FP_Gun;

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Mesh)
	class USkeletalMeshComponent* Mesh1P;

	/*Third Person Mesh*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Mesh)
	class USkeletalMeshComponent* TP_Ragdoll;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Mesh)
	class UStaticMeshComponent* Flag_Prop;

	/* Camera Rotation Sync */
	UPROPERTY(ReplicatedUsing = OnCameraRotationChanged, VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	FRotator CameraRotation;

	UPROPERTY(ReplicatedUsing = OnHealthChanged, VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float Health;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = Gameplay)
	bool bCanShoot;

	UPROPERTY(ReplicatedUsing = OnFlagHeldChanged, VisibleAnywhere, BlueprintReadOnly, Category = Gameplay)
	class ACTF_Flag* FlagHeld;


protected:
	
	/** Fires a projectile. */
	void OnFire();

	/** Resets HMD orientation and position in VR. */
	void OnResetVR();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	struct TouchData
	{
		TouchData() { bIsPressed = false;Location=FVector::ZeroVector;}
		bool bIsPressed;
		ETouchIndex::Type FingerIndex;
		FVector Location;
		bool bMoved;
	};
	void BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location);
	TouchData	TouchItem;
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	/* 
	 * Configures input for touchscreen devices if there is a valid touch interface for doing so 
	 *
	 * @param	InputComponent	The input component pointer to bind controls to
	 * @returns true if touch controls were enabled.
	 */
	bool EnableTouchscreenMovement(UInputComponent* InputComponent);

public:
	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	UFUNCTION()
	void OnCompHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
public:
	//Projectile

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFireProjectile(const FVector& SpawnLocation, const FRotator& SpawnRotation);
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastFireProjectile(const FVector& SpawnLocation, const FRotator& SpawnRotation);

	void FireProjectile(const FVector& SpawnLocation, const FRotator& SpawnRotation, const TSubclassOf<class ACTFTaskProjectile>& Projectile);

	UFUNCTION(BlueprintCallable)
	bool CanShoot();

	//Sync Camera Rotation
	UFUNCTION(Server, Reliable)
	void ServerCameraSyncRotation(const FRotator& Rotation);


	UFUNCTION()
	void OnCameraRotationChanged(const FRotator& Rotation);
	

	//HP

	UFUNCTION()
	void OnHealthChanged();

	UFUNCTION(BlueprintImplementableEvent)
	void HealthChangedCallback(const float& NewHealth);

	UFUNCTION(BlueprintCallable)
	void AddHealthPoints(float Amount);

	//Player

	UFUNCTION()
	void IsHit();


	UFUNCTION(BlueprintCallable)
	void RagdollEffect(bool IsTrue);

	UFUNCTION()
	void LocalDeath(bool IsDead);

	UFUNCTION()
	void RemoteDeath();

	UFUNCTION()
	void RemoteRespawn();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRespawnPlayer();

	//GameFlow

	UFUNCTION(Server, Reliable)
	void ServerPlayerIsReadyNotify();

	UFUNCTION()
	void InitializePlayer();

	UFUNCTION(Client, Reliable)
	void GameEnded();

	UFUNCTION()
	void OnSessionEnded(bool Successful);
	
	//Flag

	UFUNCTION()
	void OnFlagHeldChanged();

	UFUNCTION()
	void FlagVisibility();

	UFUNCTION()
	void SetFlag(class ACTF_Flag* Flag);
};

