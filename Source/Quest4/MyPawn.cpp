// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPawn.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputAction.h"
#include "InputMappingContext.h"

// Sets default values
AMyPawn::AMyPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	SetRootComponent(CapsuleComponent);
	CapsuleComponent->InitCapsuleSize(42.0f, 96.0f);
	CapsuleComponent->SetSimulatePhysics(false);
	CapsuleComponent->SetCollisionProfileName(TEXT("Pawn"));

	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	SkeletalMeshComponent->SetupAttachment(CapsuleComponent);
	SkeletalMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -96.0f));
	SkeletalMeshComponent->SetSimulatePhysics(false);

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(CapsuleComponent);
	SpringArmComponent->TargetArmLength = 350.0f;
	SpringArmComponent->SetRelativeRotation(FRotator(CameraPitch, 0.0f, 0.0f));
	SpringArmComponent->bUsePawnControlRotation = false;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);

	InputMappingContext = LoadObject<UInputMappingContext>(nullptr, TEXT("/Game/Input/IMC_Pawn.IMC_Pawn"));
	if (!InputMappingContext)
	{
		InputMappingContext = LoadObject<UInputMappingContext>(nullptr, TEXT("/Game/Input/IMC_PawnControls.IMC_PawnControls"));
	}
	if (!InputMappingContext)
	{
		InputMappingContext = LoadObject<UInputMappingContext>(nullptr, TEXT("/Game/Input/IMC_Default.IMC_Default"));
	}
	if (!InputMappingContext)
	{
		InputMappingContext = LoadObject<UInputMappingContext>(nullptr, TEXT("/Game/Input/NewFolder/IMC_PawnControls.IMC_PawnControls"));
	}

	MoveAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/Input/IA_Move.IA_Move"));
	
	LookAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/Input/IA_Look.IA_Look"));
	
	VerticalAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/Input/IA_Vertical.IA_Vertical"));
	
	RollAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/Input/IA_Roll.IA_Roll"));

	GravityToggleAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/Input/IA_GravityToggle.IA_GravityToggle"));
}

// Called when the game starts or when spawned
void AMyPawn::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				if (InputMappingContext)
				{
					InputSubsystem->AddMappingContext(InputMappingContext, 0);
				}
			}
		}

		PlayerController->SetShowMouseCursor(false);
		PlayerController->SetInputMode(FInputModeGameOnly());
	}
}

// Called every frame
void AMyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateGroundState();
	ApplyRotation(DeltaTime);
	ApplyMovement(DeltaTime);
	ApplyGravity(DeltaTime);
	UpdateGroundStateScreenLog();
}

// Called to bind functionality to input
void AMyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (MoveAction)
		{
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyPawn::Move);
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &AMyPawn::StopMove);
		}

		if (LookAction)
		{
			EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyPawn::Look);
			EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Completed, this, &AMyPawn::StopLook);
		}

		if (VerticalAction)
		{
			EnhancedInputComponent->BindAction(VerticalAction, ETriggerEvent::Triggered, this, &AMyPawn::MoveVertical);
			EnhancedInputComponent->BindAction(VerticalAction, ETriggerEvent::Completed, this, &AMyPawn::StopVertical);
		}
		
		if (RollAction)
		{
			EnhancedInputComponent->BindAction(RollAction, ETriggerEvent::Triggered, this, &AMyPawn::Roll);
			EnhancedInputComponent->BindAction(RollAction, ETriggerEvent::Completed, this, &AMyPawn::StopRoll);
		}

		if (GravityToggleAction)
		{
			EnhancedInputComponent->BindAction(GravityToggleAction, ETriggerEvent::Started, this, &AMyPawn::ToggleGravity);
		}
	}
}

void AMyPawn::Move(const FInputActionValue& Value)
{
	MoveInput = Value.Get<FVector2D>();
}

void AMyPawn::Look(const FInputActionValue& Value)
{
	LookInput = Value.Get<FVector2D>();
}

void AMyPawn::MoveVertical(const FInputActionValue& Value)
{
	VerticalInput = Value.Get<float>();
}

void AMyPawn::Roll(const FInputActionValue& Value)
{
	RollInput = Value.Get<float>();
}

void AMyPawn::ToggleGravity(const FInputActionValue& Value)
{
	bIgnoreGravity = !bIgnoreGravity;
	if (bIgnoreGravity)
	{
		FallingVelocity = 0.0f;
	}
}

void AMyPawn::StopMove()
{
	MoveInput = FVector2D::ZeroVector;
}

void AMyPawn::StopLook()
{
	LookInput = FVector2D::ZeroVector;
}

void AMyPawn::StopVertical()
{
	VerticalInput = 0.0f;
}

void AMyPawn::StopRoll()
{
	RollInput = 0.0f;
}

void AMyPawn::UpdateGroundState()
{
	FHitResult HitResult;
	const FVector Start = GetActorLocation();
	const FVector End = Start - FVector(0.0f, 0.0f, CapsuleComponent->GetScaledCapsuleHalfHeight() + 6.0f);
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(PawnGroundTrace), false, this);

	bIsGrounded = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, QueryParams);
}

void AMyPawn::UpdateGroundStateScreenLog()
{
	if (bHasShownGroundState && bLastGroundedForScreenLog == bIsGrounded)
	{
		return;
	}

	if (GEngine)
	{
		const FString StateText = bIsGrounded ? TEXT("Grounded: TRUE") : TEXT("Grounded: FALSE");
		const FColor TextColor = bIsGrounded ? FColor::White : FColor::Red;
		GEngine->AddOnScreenDebugMessage(1, 2.0f, TextColor, StateText);
	}

	bLastGroundedForScreenLog = bIsGrounded;
	bHasShownGroundState = true;
}

void AMyPawn::ApplyMovement(float DeltaTime)
{
	const float CurrentMoveSpeed = bIsGrounded ? MoveSpeed : MoveSpeed * AirControlRatio;
	const FVector LocalMovement(
		MoveInput.Y * CurrentMoveSpeed * DeltaTime,
		MoveInput.X * CurrentMoveSpeed * DeltaTime,
		VerticalInput * CurrentMoveSpeed * DeltaTime
	);

	if (!LocalMovement.IsNearlyZero())
	{
		FHitResult HitResult;
		AddActorLocalOffset(LocalMovement, true, &HitResult);

		if (HitResult.IsValidBlockingHit() && HitResult.ImpactNormal.Z > 0.5f)
		{
			bIsGrounded = true;
			FallingVelocity = 0.0f;
		}
	}
}

void AMyPawn::ApplyRotation(float DeltaTime)
{
	const float YawDelta = LookInput.X * LookSpeed * DeltaTime;
	const float PitchDelta = -LookInput.Y * LookSpeed * DeltaTime;
	const float RollDelta = RollInput * RollSpeed * DeltaTime;

	if (!FMath::IsNearlyZero(YawDelta) || !FMath::IsNearlyZero(RollDelta))
	{
		AddActorWorldRotation(FRotator(0.0f, YawDelta, 0.0f), true);
		AddActorLocalRotation(FRotator(0.0f, 0.0f, RollDelta), true);
	}

	if (!FMath::IsNearlyZero(PitchDelta))
	{
		CameraPitch = FMath::Clamp(CameraPitch + PitchDelta, -80.0f, 80.0f);
		SpringArmComponent->SetRelativeRotation(FRotator(CameraPitch, 0.0f, 0.0f));
	}
}

void AMyPawn::ApplyGravity(float DeltaTime)
{
	if (bIgnoreGravity)
	{
		FallingVelocity = 0.0f;
		return;
	}

	if (bIsGrounded && VerticalInput <= 0.0f)
	{
		FallingVelocity = 0.0f;
		return;
	}

	FallingVelocity += GravityAcceleration * DeltaTime;

	FHitResult HitResult;
	AddActorWorldOffset(FVector(0.0f, 0.0f, FallingVelocity * DeltaTime), true, &HitResult);

	if (HitResult.IsValidBlockingHit() && HitResult.ImpactNormal.Z > 0.5f)
	{
		bIsGrounded = true;
		FallingVelocity = 0.0f;
	}
}
