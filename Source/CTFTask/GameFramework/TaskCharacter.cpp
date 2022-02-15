// Copyright Epic Games, Inc. All Rights Reserved.

#include "TaskCharacter.h"
#include "TaskProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId
#include "Net/UnrealNetwork.h"
#include "TaskGameModeGameplay.h"
#include "TaskHUD.h"
#include "CTF_PlayerState.h"
#include "CTF_Base.h"
#include "CTF_Flag.h"
#include "CTF_GameInstanceOnline.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// ACTFTaskCharacter

ACTFTaskCharacter::ACTFTaskCharacter()
{
	//Set Replicated to true
	bReplicates = true;

	//Player can shoot
	bCanShoot = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &ACTFTaskCharacter::OnCompHit);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(false);			
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));


	//Create a skeletal mesh component for the third person body
	TP_Body = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TP_Body"));
	TP_Body->SetOnlyOwnerSee(false);
	TP_Body->SetOwnerNoSee(true);
	TP_Body->SetupAttachment(GetCapsuleComponent());
	TP_Body->bCastDynamicShadow = false;
	TP_Body->CastShadow = false;
	TP_Body->SetRelativeRotation(FRotator(0.0f, 0.0f, -90.0f));
	TP_Body->SetRelativeLocation(FVector(-90.0f, 0.0f, 0.0f));
	

	// Create a gun mesh component
	TP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TP_Gun"));
	TP_Gun->SetOnlyOwnerSee(false);
	TP_Gun->bCastDynamicShadow = false;
	TP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	TP_Gun->SetupAttachment(RootComponent);



	TP_Ragdoll = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TP_Ragdoll"));
	TP_Ragdoll->SetOnlyOwnerSee(false);
	TP_Ragdoll->SetOwnerNoSee(true);
	TP_Ragdoll->SetupAttachment(GetCapsuleComponent());
	TP_Ragdoll->bCastDynamicShadow = false;
	TP_Ragdoll->CastShadow = false;
	TP_Ragdoll->SetRelativeRotation(FRotator(0.0f, 0.0f, -90.0f));
	TP_Ragdoll->SetRelativeLocation(FVector(-90.0f, 0.0f, 0.0f));

	Flag_Prop = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Flag_Prop"));
	Flag_Prop->SetupAttachment(GetCapsuleComponent());

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun 
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.

	// Create VR Controllers.
	R_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("R_MotionController"));
	R_MotionController->MotionSource = FXRMotionControllerBase::RightHandSourceId;
	R_MotionController->SetupAttachment(RootComponent);
	L_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("L_MotionController"));
	L_MotionController->SetupAttachment(RootComponent);

	// Create a gun and attach it to the right-hand VR controller.
	// Create a gun mesh component
	VR_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("VR_Gun"));
	VR_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	VR_Gun->bCastDynamicShadow = false;
	VR_Gun->CastShadow = false;
	VR_Gun->SetupAttachment(R_MotionController);
	VR_Gun->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	VR_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("VR_MuzzleLocation"));
	VR_MuzzleLocation->SetupAttachment(VR_Gun);
	VR_MuzzleLocation->SetRelativeLocation(FVector(0.000004, 53.999992, 10.000000));
	VR_MuzzleLocation->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));		// Counteract the rotation of the VR gun model.

	// Uncomment the following line to turn motion controllers on by default:
	//bUsingMotionControllers = true;
}

void ACTFTaskCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Set Health to initial value
	Health = 100.0f;

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

	TP_Gun->AttachToComponent(TP_Body, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));


	// Show or hide the two versions of the gun based on whether or not we're using motion controllers.
	if (bUsingMotionControllers)
	{
		VR_Gun->SetHiddenInGame(false, true);
		Mesh1P->SetHiddenInGame(true, true);
	}
	else
	{
		VR_Gun->SetHiddenInGame(true, true);
		Mesh1P->SetHiddenInGame(false, true);
	}
	

	if (IsLocallyControlled())
	{
		//Notify server that this player is ready
		ServerPlayerIsReadyNotify();
	}

	InitializePlayer();
}
//////////////////////////////////////////////////////////////////////////
// Input

void ACTFTaskCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACTFTaskCharacter, CameraRotation);
	DOREPLIFETIME(ACTFTaskCharacter, Health);
	DOREPLIFETIME(ACTFTaskCharacter, bCanShoot);
	DOREPLIFETIME(ACTFTaskCharacter, FlagHeld);
}

void ACTFTaskCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ACTFTaskCharacter::OnFire);

	// Enable touchscreen input
	EnableTouchscreenMovement(PlayerInputComponent);

	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ACTFTaskCharacter::OnResetVR);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &ACTFTaskCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACTFTaskCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ACTFTaskCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ACTFTaskCharacter::LookUpAtRate);
}

void ACTFTaskCharacter::OnFire()
{
	if(CanShoot())
	{
		const FRotator SpawnRotation = GetControlRotation();
		// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
		const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

		// try and play a firing animation if specified
		if (FireAnimation != NULL)
		{
			// Get the animation object for the arms mesh
			UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
			if (AnimInstance != NULL)
			{
				AnimInstance->Montage_Play(FireAnimation, 1.f);
			}
		}

		FireProjectile(SpawnLocation, SpawnRotation, FakeProjectileClass);

		ServerFireProjectile(SpawnLocation, SpawnRotation);
	}
}

void ACTFTaskCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ACTFTaskCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnFire();
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void ACTFTaskCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	TouchItem.bIsPressed = false;
}


void ACTFTaskCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ACTFTaskCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ACTFTaskCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ACTFTaskCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());

	ServerCameraSyncRotation(GetControlRotation());
}

bool ACTFTaskCharacter::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ACTFTaskCharacter::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &ACTFTaskCharacter::EndTouch);

		//Commenting this out to be more consistent with FPS BP template.
		//PlayerInputComponent->BindTouch(EInputEvent::IE_Repeat, this, &ACTFTaskCharacter::TouchUpdate);
		return true;
	}
	
	return false;
}


void ACTFTaskCharacter::FireProjectile(const FVector& SpawnLocation, const FRotator& SpawnRotation,const TSubclassOf<class ACTFTaskProjectile>& Projectile)
{
	// try and fire a projectile
	if (Projectile != NULL)
	{
		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			if (bUsingMotionControllers)
			{
				//const FRotator SpawnRotation = VR_MuzzleLocation->GetComponentRotation();
				//const FVector SpawnLocation = VR_MuzzleLocation->GetComponentLocation();
				World->SpawnActor<ACTFTaskProjectile>(Projectile, SpawnLocation, SpawnRotation);
			}
			else
			{
				//const FRotator SpawnRotation = GetControlRotation();
				// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
				//const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

				//Set Spawn Collision Handling Override
				FActorSpawnParameters ActorSpawnParams;
				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

				// spawn the projectile at the muzzle
				ACTFTaskProjectile* SpawnedProjectile = World->SpawnActor<ACTFTaskProjectile>(Projectile, SpawnLocation, SpawnRotation, ActorSpawnParams);
				if(SpawnedProjectile != NULL)
				{
					SpawnedProjectile->ProjectileShooter = this;
				}
			}
		}
	}
}

void ACTFTaskCharacter::ServerFireProjectile_Implementation(const FVector& SpawnLocation, const FRotator& SpawnRotation)
{
	if(CanShoot())
	{
		FireProjectile(SpawnLocation, SpawnRotation, ProjectileClass);
	}
}

bool ACTFTaskCharacter::ServerFireProjectile_Validate(const FVector& SpawnLocation, const FRotator& SpawnRotation)
{
	return true;
}

void ACTFTaskCharacter::MulticastFireProjectile_Implementation(const FVector& SpawnLocation, const FRotator& SpawnRotation)
{
	FireProjectile(SpawnLocation, SpawnRotation, ProjectileClass);
}

void ACTFTaskCharacter::ServerCameraSyncRotation_Implementation(const FRotator& Rotation)
{
	CameraRotation = Rotation;
}

void ACTFTaskCharacter::OnCameraRotationChanged(const FRotator& Rotation)
{
	if (!IsLocallyControlled())
	{
		FirstPersonCameraComponent->SetRelativeRotation(Rotation);
	}
}

void ACTFTaskCharacter::OnHealthChanged()
{
	if (IsLocallyControlled())
	{
		ATaskHUD* PlayerHud = (ATaskHUD* )UGameplayStatics::GetPlayerController(this, 0)->GetHUD();
		if(PlayerHud)
		{
			PlayerHud->UpdateHealth(Health / 100.0f);
		}

		//Check if player is dead
		if (Health <= 0.0f)
		{
			RemoteDeath();
			LocalDeath(true);
		}
	}
	else
	{
		//Check if player is dead
		if (Health <= 0.0f)
		{
			RemoteDeath();
		}
	}
}

void ACTFTaskCharacter::AddHealthPoints(float Amount)
{
	if(GetLocalRole() == ROLE_Authority)
	{
		float TempHealth = Health + Amount;

		if (TempHealth > 100.0f)
		{
			Health = 100.0f;
		}
		else if (TempHealth < 0.0f)
		{
			Health = 0.0f;
		}
		else
		{
			Health += Amount;
		}
		OnHealthChanged();
	}
}

bool ACTFTaskCharacter::CanShoot()
{
	return Health > 0.0f && bCanShoot;
}

void ACTFTaskCharacter::ServerPlayerIsReadyNotify_Implementation()
{
	ATaskGameModeGameplay* GameMode = (ATaskGameModeGameplay* )GetWorld()->GetAuthGameMode();
	if(GameMode != nullptr)
	{
		GameMode->InstanceReady();
	}
}


void ACTFTaskCharacter::RagdollEffect(bool IsTrue)
{
	//Reset Ragdoll Location
	TP_Ragdoll->SetWorldTransform(GetActorTransform());

	TP_Ragdoll->SetVisibility(IsTrue);
	TP_Ragdoll->SetAllBodiesSimulatePhysics(IsTrue);

	//Propogate to affect the gun
	TP_Body->SetVisibility(!IsTrue, true);
}

/*This is what the player will do locally to their own character on death*/
void ACTFTaskCharacter::LocalDeath(bool IsDead)
{
	Mesh1P->SetOwnerNoSee(IsDead);
	FP_Gun->SetOwnerNoSee(IsDead);

	if (IsDead)
	{
		ATaskHUD* PlayerHud = (ATaskHUD* )UGameplayStatics::GetPlayerController(this, 0)->GetHUD();
		if (PlayerHud != nullptr)
		{
			PlayerHud->ShowScreenUI(IsDead);
			PlayerHud->ShowCursor(IsDead);
		}
	}
}
/*This is what other remote clients see on character death including server*/
void ACTFTaskCharacter::RemoteDeath()
{
	RagdollEffect(true);

	if (GetLocalRole() == ROLE_Authority)
	{
		//Tell game mode about this death
		ATaskGameModeGameplay* GameMode = (ATaskGameModeGameplay* )GetWorld()->GetAuthGameMode();
		if(GameMode != nullptr)
		{
			GameMode->PlayerDeathHandling(this);
		}
		//Stop player from being able to move
		//GetCharacterMovement()->DisableMovement();
		//Respawn player in 3 seconds

		//FTimerHandle UnusedHandle;
		//GetWorldTimerManager().SetTimer(UnusedHandle, this, &ACTFTaskCharacter::RemoteRespawn, 3.0f, false);

	}
}
/*This is how the server respawns a player*/

void ACTFTaskCharacter::RemoteRespawn()
{
/*
	//Enable Movement
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	//Adjust health
	AddHealthPoints(100.0f);
	//Turn off ragdoll on server
	RagdollEffect(false);
	//Tell clients to setup client-side respawn
	MulticastRespawnPlayer();
	//Teleport player to the correct position
	ACTF_PlayerState* PS = (ACTF_PlayerState* )GetPlayerState();
	if (PS)
	{
		if (PS->IsTeamA)
		{
			//Teleport player to Team A's Spawn Point
			SetActorLocation(FVector(-1360, 1110, 200), false, nullptr, ETeleportType::TeleportPhysics);
		}
		else
		{
			//Teleport player to Team B's Spawn Point
			SetActorLocation(FVector(1220.0, -1040, 200), false, nullptr, ETeleportType::TeleportPhysics);
		}
	}
*/
}

void ACTFTaskCharacter::MulticastRespawnPlayer_Implementation()
{
	//Turn off ragdoll for this client-side
	RagdollEffect(false);
	
	if (IsLocallyControlled())
	{
		//Disable local death on locally controlled player
		LocalDeath(false);
	}
}

void ACTFTaskCharacter::IsHit()
{
	if (IsLocallyControlled() == false)
	{
		if (HitAnimation != NULL)
		{
			// Get the animation object for the arms mesh
			UAnimInstance* AnimInstance = TP_Body->GetAnimInstance();
			if (AnimInstance != NULL)
			{
				AnimInstance->Montage_Play(HitAnimation, 2.f);
			}
		}
	}
}

void ACTFTaskCharacter::InitializePlayer()
{
	ACTF_PlayerState* PS = (ACTF_PlayerState*)GetPlayerState();
	
	if(PS)
	{
		if (PS->IsTeamA)
		{
			//Apply Materials
			TP_Body->SetMaterial(0, RedBodyMaterial);
			TP_Ragdoll->SetMaterial(0, RedBodyMaterial);
			Flag_Prop->SetMaterial(0, BlueFlagMaterial);
		}
		else
		{
			//Apply Materials
			TP_Body->SetMaterial(0, BlueBodyMaterial);
			TP_Ragdoll->SetMaterial(0, BlueBodyMaterial);
			Flag_Prop->SetMaterial(0, RedFlagMaterial);
		}
	}
}

void ACTFTaskCharacter::OnCompHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		//Flag

		if(OtherActor->GetClass()->IsChildOf(ACTF_Flag::StaticClass()))
		{
			ACTF_Flag* Flag = (ACTF_Flag* )OtherActor;
			if (Flag != nullptr)
			{
				ATaskGameModeGameplay* GameMode = (ATaskGameModeGameplay* )GetWorld()->GetAuthGameMode();
				if(GameMode != nullptr)
				{
					GameMode->PlayerFlagInteract(this, Flag);
				}
			}
		}
		//Base
		if(OtherActor->GetClass()->IsChildOf(ACTF_Base::StaticClass()))
		{
			ACTF_Base* Base = (ACTF_Base* )OtherActor;
			if (Base != nullptr)
			{
				ATaskGameModeGameplay* GameMode = (ATaskGameModeGameplay* )GetWorld()->GetAuthGameMode();
				if(GameMode != nullptr)
				{
					GameMode->PlayerBaseInteract(this, Base);
				}
			}
		}

	}
}

void ACTFTaskCharacter::OnFlagHeldChanged()
{
	FlagVisibility();
}

void ACTFTaskCharacter::FlagVisibility()
{
	Flag_Prop->SetVisibility(FlagHeld != nullptr);
}

void ACTFTaskCharacter::SetFlag(class ACTF_Flag* Flag)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		FlagHeld = Flag;

		FlagVisibility();
	}
}

void ACTFTaskCharacter::GameEnded_Implementation()
{
	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		//Destroy session
		UCTF_GameInstanceOnline* GameInstance = (UCTF_GameInstanceOnline* )GetGameInstance();
		if(GameInstance != nullptr)
		{
			GameInstance->OnDestroySessionCompleteEvent.AddDynamic(this, &ACTFTaskCharacter::OnSessionEnded);

			GameInstance->DestroySession();
		}
	}
}

void ACTFTaskCharacter::OnSessionEnded(bool Successful)
{
	if(Successful)
	{
		UGameplayStatics::OpenLevel(GetWorld(), FName("MainMenu"));
	}
}
