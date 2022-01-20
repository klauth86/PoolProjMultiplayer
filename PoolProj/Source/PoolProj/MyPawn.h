// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "MyPawn.generated.h"

class UStaticMeshComponent;
class USkeletalMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputComponent;
class USoundBase;

UENUM(BlueprintType)
enum class MyPawnState : uint8
{
	ACTIVE UMETA(DisplayName = "ACTIVE"),
	LAUNCHED UMETA(DisplayName = "LAUNCHED"),
	PENDING UMETA(DisplayName = "PENDING")
};

UCLASS()
class POOLPROJ_API AMyPawn : public APawn
{
	GENERATED_BODY()

public:

	AMyPawn();

	void BeginPlay() override;

	void Tick(float DeltaSeconds) override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_AddMovementInput(FVector val);
	void Server_AddMovementInput_Implementation(FVector val) { ControlInputVector += val; }
	bool Server_AddMovementInput_Validate(FVector val) { return true; }









protected:

	void CommonMoveForward(float Val);
	void MoveForward(float Val);

	void CommonMoveRight(float Val);
	void MoveRight(float Val);

	UFUNCTION(Server, Reliable, WithValidation)
	void CommonStopFire();
	void CommonStopFire_Implementation();
	bool CommonStopFire_Validate();
	void StopFire();

	UFUNCTION(Server, Reliable, WithValidation)
	void CommonStartFire();
	void CommonStartFire_Implementation();
	bool CommonStartFire_Validate();
	void StartFire();

	void SetupBodyInstance();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetYaw(float value);
	void ServerSetYaw_Implementation(float value);
	bool ServerSetYaw_Validate(float value);

	void SetYaw();

	UFUNCTION()
	void OnRep_SetYaw();

	void StopMovement();

protected:

	static TMap<int, AMyPawn*> Instances;

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* CollisionComponent;

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USkeletalMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = MyPawn, meta = (AllowPrivateAccess = "true"))
		FVector StartLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = MyPawn, meta = (AllowPrivateAccess = "true"))
		FRotator StartRotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USoundBase* FallSfx;

	UPROPERTY(EditAnywhere, Category = MyPawn)
		float Sight = 5000; // In Sm

	UPROPERTY(EditAnywhere, Category = MyPawn)
		float Mass = 1; // In Kg

	UPROPERTY(EditAnywhere, Category = MyPawn)
		float ForceAmount = 760000; // IN Newtons
	// https://billiards.colostate.edu/technical_proofs/new/TP_B-20.pdf

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
		MyPawnState State = MyPawnState::ACTIVE;

	UPROPERTY(ReplicatedUsing = OnRep_SetYaw)
		float Yaw;
};