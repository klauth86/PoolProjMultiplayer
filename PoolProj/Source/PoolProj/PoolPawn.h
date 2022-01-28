// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "PoolPawn.generated.h"

class UGameWidget;
class UCameraComponent;
class ARepresenter;

UCLASS()
class POOLPROJ_API APoolPawn : public APawn
{
	GENERATED_BODY()

public:

	APoolPawn();

public:

	virtual void BeginPlay() override;

	virtual void EndPlay(EEndPlayReason::Type endPlayReason) override;

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	bool IsPrepared() const { return bIsPrepared; }

	UFUNCTION(Server, Reliable)
	void Server_SetAsPrepared();
	void Server_SetAsPrepared_Implementation() { bIsPrepared = true; }

	bool IsActive() const { return bIsActive; }

	void SetIsActive(bool isActive);

	FVector GetRepresenterOffset(FRotator rotation) const;

protected:

	void MoveForward(float Val);

	void MoveRight(float Val);

	UFUNCTION(Server, Reliable)
	void Server_StartFire();	
	void Server_StartFire_Implementation();

	UFUNCTION(Server, Reliable)
	void Server_StopFire();
	void Server_StopFire_Implementation();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_AddMovementInput(FVector val);
	void Server_AddMovementInput_Implementation(FVector val) { ControlInputVector += val; }
	bool Server_AddMovementInput_Validate(FVector val) { return true; }

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_AddControllerYawInput(float val);
	void Server_AddControllerYawInput_Implementation(float val) { YawInput += val; }
	bool Server_AddControllerYawInput_Validate(float val) { return true; }

	float ConsumeYawInput() { float yawInput = YawInput; YawInput = 0; return yawInput; }

	UFUNCTION(Server, Reliable)
	void Server_Skip();
	void Server_Skip_Implementation();

	void OnShot(UClass* ballClass) { Shots.Add(ballClass); }

	void Client_OnPrepared(UObject* widgetOwner);

	UFUNCTION()
		void OnRep_IsPrepared();

	UFUNCTION()
		void OnRep_IsActive();

	UFUNCTION()
		void OnRep_Shots();

	void InitUI();

	void UnInitUI();

	void AttachToRepresneter();

	void EndTurn();

protected:

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"), Category = "PoolPawn")
		UCameraComponent* CameraComponent;

	UPROPERTY(EditDefaultsOnly, Category = "PoolPawn")
		TSubclassOf<UGameWidget> GameWidgetClass;

	UPROPERTY()
		UGameWidget* GameWidget;

	UPROPERTY(EditDefaultsOnly, Category = "PoolPawn")
		TSubclassOf<ARepresenter> RepresenterClass;

	FVector PreLaunchLocation;
	
	FRotator PreLaunchRotation;

	FVector PreFloatLocation;

	FRotator PreFloatRotation;

	UPROPERTY(EditDefaultsOnly, Category = "PoolPawn")
		float RespawnDistance;

	UPROPERTY(EditDefaultsOnly, Category = "PoolPawn")
		float MaxSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "PoolPawn")
		float TargetLength;

	UPROPERTY(EditDefaultsOnly, Category = "PoolPawn")
		float TargetAngle;

	float RestorePositionTimeLeft;

	UPROPERTY(EditDefaultsOnly, Category = "PoolPawn")
		float RestorePositionTime;

	float YawInput;

	float StrengthTimeLeft;

	UPROPERTY(EditDefaultsOnly, Category = "PoolPawn")
		float StrengthTime;

	UPROPERTY(ReplicatedUsing = OnRep_Shots)
		TArray<UClass*> Shots;

	UPROPERTY(Replicated)
		float Strength;

	UPROPERTY(Replicated)
		ARepresenter* Representer;

	UPROPERTY(ReplicatedUsing = OnRep_IsPrepared)
		uint8 bIsPrepared : 1;

	UPROPERTY(ReplicatedUsing = OnRep_IsActive)
		uint8 bIsActive : 1;

	uint8 bIsFloatingToRepresenter : 1;

	uint8 bHasBeenLaunched : 1;

	uint8 bIsActionPressed : 1;

	uint8 bIsActionPressedLastFrame : 1;
};