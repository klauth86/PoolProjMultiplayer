// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "PoolPawn.generated.h"

class UStaticMeshComponent;
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class POOLPROJ_API APoolPawn : public APawn
{
	GENERATED_BODY()

public:

	APoolPawn();

public:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
		void OnRep_IsPreparing();

	bool IsPreparing() const { return bIsPreparing; }

	bool IsActive() const { return bIsActive; }

	void SetIsActive(bool isActive) { bIsActive = isActive; }

protected:

	void MoveForward(float Val);

	void MoveRight(float Val);

	void StartFire();

	void StopFire();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_AddMovementInput(FVector val);
	void Server_AddMovementInput_Implementation(FVector val) { ControlInputVector += val; }
	bool Server_AddMovementInput_Validate(FVector val) { return true; }

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Character")
		float MaxSpeed;

	UPROPERTY(VisibleAnywhere, Category = "Character", meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, Category = "Camera", meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = "Camera", meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

	UPROPERTY(ReplicatedUsing = OnRep_IsPreparing)
		uint8 bIsPreparing : 1;

	UPROPERTY(Replicated)
		uint8 bIsActive : 1;
};
