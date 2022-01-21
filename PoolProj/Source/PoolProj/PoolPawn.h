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
		void OnRep_IsPrepared();

	bool IsPrepared() const { return bIsPrepared; }

	UFUNCTION()
		void SetAsPrepared() { UE_LOG(LogTemp, Warning, TEXT("PoolPawn: %s Finish preparing..."), *GetName())
		bIsPrepared = true; }

	UFUNCTION()
		void OnRep_IsActive();

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

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_AddControllerYawInput(float val);
	void Server_AddControllerYawInput_Implementation(float val) { YawInput += val; }
	bool Server_AddControllerYawInput_Validate(float val) { return true; }

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Skip();
	void Server_Skip_Implementation();
	bool Server_Skip_Validate() { return true; }

	float ConsumeYawInput() { float yawInput = YawInput; YawInput = 0; return yawInput; }

protected:

	UPROPERTY(VisibleAnywhere, Category = "Character", meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, Category = "Camera", meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = "Camera", meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

	UPROPERTY(EditDefaultsOnly, Category = "Character")
		float MaxSpeed;

	float YawInput;

	UPROPERTY(ReplicatedUsing = OnRep_IsPrepared)
		uint8 bIsPrepared : 1;

	UPROPERTY(ReplicatedUsing = OnRep_IsActive)
		uint8 bIsActive : 1;
};