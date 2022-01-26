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

	UFUNCTION(Server, Reliable)
	void Server_SetAsPrepared();
	void Server_SetAsPrepared_Implementation()
	{
		UE_LOG(LogTemp, Warning, TEXT("*** %s: %s is prepared!"), *(GetWorld()->GetNetMode() == ENetMode::NM_Client ? FString::Printf(TEXT("Client %d: "), GPlayInEditorID) : FString("Server")), *GetName());
		bIsPrepared = true;
	}

	UFUNCTION()
		void OnRep_IsActive();

	bool IsActive() const { return bIsActive; }

	void SetIsActive(bool isActive) { bIsActive = isActive; }

	float GetTargetLength() const { return TargetLength; }

	float GetTargetAngle() const { return TargetAngle; }

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

	UFUNCTION(Server, Reliable)
	void Server_Skip();
	void Server_Skip_Implementation();

	float ConsumeYawInput() { float yawInput = YawInput; YawInput = 0; return yawInput; }

protected:

	UPROPERTY(EditDefaultsOnly, Category = "PoolPawn")
		TSubclassOf<AActor> RepresenterClass;

	UPROPERTY(Replicated)
		AActor* Representer;

	UPROPERTY(EditDefaultsOnly, Category = "PoolPawn")
		float MaxSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "PoolPawn")
		float TargetLength;

	UPROPERTY(EditDefaultsOnly, Category = "PoolPawn")
		float TargetAngle;

	float YawInput;

	UPROPERTY(ReplicatedUsing = OnRep_IsPrepared)
		uint8 bIsPrepared : 1;

	UPROPERTY(ReplicatedUsing = OnRep_IsActive)
		uint8 bIsActive : 1;
};