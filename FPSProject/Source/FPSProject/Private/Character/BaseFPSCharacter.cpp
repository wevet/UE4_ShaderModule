// Copyright Epic Games, Inc. All Rights Reserved.

#include "Character/BaseFPSCharacter.h"
#include "Tools/BulletProjectile.h"

#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);


ABaseFPSCharacter::ABaseFPSCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	FirstPersonCameraComponent = ObjectInitializer.CreateDefaultSubobject<UCameraComponent>(this, TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	Mesh1P = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeLocation(FVector(3.293318f, -4.998758f, -161.327179f));
	Mesh1P->SetRelativeRotation(FRotator(5.200002f, 1.899995f, -19.189999f));

	FP_Gun = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(false);
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	FP_Gun->SetupAttachment(RootComponent);

	FP_MuzzleLocation = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	R_MotionController = ObjectInitializer.CreateDefaultSubobject<UMotionControllerComponent>(this, TEXT("R_MotionController"));
	R_MotionController->MotionSource = FXRMotionControllerBase::RightHandSourceId;
	R_MotionController->SetupAttachment(RootComponent);
	L_MotionController = ObjectInitializer.CreateDefaultSubobject<UMotionControllerComponent>(this, TEXT("L_MotionController"));
	L_MotionController->SetupAttachment(RootComponent);

	VR_Gun = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("VR_Gun"));
	VR_Gun->SetOnlyOwnerSee(false);
	VR_Gun->bCastDynamicShadow = false;
	VR_Gun->CastShadow = false;
	VR_Gun->SetupAttachment(R_MotionController);
	VR_Gun->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	VR_MuzzleLocation = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("VR_MuzzleLocation"));
	VR_MuzzleLocation->SetupAttachment(VR_Gun);
	VR_MuzzleLocation->SetRelativeLocation(FVector(0.000004, 53.999992, 10.000000));
	VR_MuzzleLocation->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}


void ABaseFPSCharacter::BeginPlay()
{
	Super::BeginPlay();

	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

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
}


void ABaseFPSCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ABaseFPSCharacter::OnFire);

	EnableTouchscreenMovement(PlayerInputComponent);

	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ABaseFPSCharacter::OnResetVR);

	PlayerInputComponent->BindAxis("MoveForward", this, &ABaseFPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABaseFPSCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ABaseFPSCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ABaseFPSCharacter::LookUpAtRate);
}


void ABaseFPSCharacter::OnFire()
{
	if (ProjectileClass == nullptr)
	{
		return;
	}

	UWorld* const World = GetWorld();
	if (World == nullptr)
	{
		return;
	}


	if (bUsingMotionControllers)
	{
		const FRotator SpawnRotation = VR_MuzzleLocation->GetComponentRotation();
		const FVector SpawnLocation = VR_MuzzleLocation->GetComponentLocation();
		World->SpawnActor<ABulletProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);
	}
	else
	{
		const FRotator SpawnRotation = GetControlRotation();
		const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

		World->SpawnActor<ABulletProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
	}


	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	if (FireAnimation)
	{
		if (UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance())
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}


#pragma region VRInput
void ABaseFPSCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}
#pragma endregion


#pragma region TouchInput
void ABaseFPSCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed)
	{
		return;
	}

	if ((FingerIndex == TouchItem.FingerIndex) && (!TouchItem.bMoved))
	{
		OnFire();
	}

	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}


void ABaseFPSCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed)
	{
		TouchItem.bIsPressed = false;
	}

}


void ABaseFPSCharacter::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
{

	if ((TouchItem.bIsPressed) && (TouchItem.FingerIndex == FingerIndex))
	{
		if (TouchItem.bIsPressed)
		{
			if (!GetWorld())
			{
				return;
			}

			UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
			if (ViewportClient)
			{
				FVector2D ScreenSize;
				ViewportClient->GetViewportSize(ScreenSize);

				const FVector MoveDelta = (Location - TouchItem.Location);
				const FVector2D ScaledDelta = FVector2D(MoveDelta.X, MoveDelta.Y) / ScreenSize;
				const float Limit = 4.0f;


				if (FMath::Abs(ScaledDelta.X) >= Limit / ScreenSize.X)
				{
					TouchItem.bMoved = true;
					const float Value = ScaledDelta.X * BaseTurnRate;
					AddControllerYawInput(Value);
				}

				if (FMath::Abs(ScaledDelta.Y) >= Limit / ScreenSize.Y)
				{
					TouchItem.bMoved = true;
					const float Value = ScaledDelta.Y * BaseTurnRate;
					AddControllerPitchInput(Value);
				}
				TouchItem.Location = Location;
			}
			TouchItem.Location = Location;
		}
	}
}
#pragma endregion


#pragma region DefaultInput
void ABaseFPSCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		AddMovementInput(GetActorForwardVector(), Value);
	}
}


void ABaseFPSCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		AddMovementInput(GetActorRightVector(), Value);
	}
}


void ABaseFPSCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}


void ABaseFPSCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}
#pragma endregion


bool ABaseFPSCharacter::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	check(GetDefault<UInputSettings>());

	const UInputSettings* Setting = GetDefault<UInputSettings>();

	if (FPlatformMisc::SupportsTouchInput() || Setting->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ABaseFPSCharacter::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &ABaseFPSCharacter::EndTouch);
		return true;
	}

	return false;
}
