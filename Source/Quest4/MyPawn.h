// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "MyPawn.generated.h"

class UCapsuleComponent;
class USkeletalMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;

UCLASS()
class QUEST4_API AMyPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMyPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UCapsuleComponent> CapsuleComponent;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USkeletalMeshComponent> SkeletalMeshComponent;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USpringArmComponent> SpringArmComponent;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> InputMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> VerticalAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> RollAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> GravityToggleAction;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float MoveSpeed = 600.0f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float AirControlRatio = 0.4f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float LookSpeed = 90.0f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float RollSpeed = 90.0f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float GravityAcceleration = -980.0f;

	FVector2D MoveInput = FVector2D::ZeroVector;
	FVector2D LookInput = FVector2D::ZeroVector;
	float CameraPitch = -15.0f;
	float VerticalInput = 0.0f;
	float RollInput = 0.0f;
	float FallingVelocity = 0.0f;
	bool bIsGrounded = false;
	bool bIgnoreGravity = false;
	bool bHasShownGroundState = false;
	bool bLastGroundedForScreenLog = false;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void MoveVertical(const FInputActionValue& Value);
	void Roll(const FInputActionValue& Value);
	void ToggleGravity(const FInputActionValue& Value);
	void StopMove();
	void StopLook();
	void StopVertical();
	void StopRoll();
	void UpdateGroundState();
	void UpdateGroundStateScreenLog();
	void ApplyMovement(float DeltaTime);
	void ApplyRotation(float DeltaTime);
	void ApplyGravity(float DeltaTime);
};
