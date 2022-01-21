// Fill out your copyright notice in the Description page of Project Settings.

#include "PoolPawn.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Net/UnrealNetwork.h"
#include "ActionRouter.h"
#include "TimerManager.h"

APoolPawn::APoolPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CollisionComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CollisionComponent"));

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 30.0f; // The camera follows at this distance behind the character
	CameraBoom->SetRelativeRotation(FRotator(-30, 0, 0));
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->bInheritPitch = false;
	CameraBoom->bDoCollisionTest = false;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	MaxSpeed = 1200;
	bIsPrepared = false;
	bIsActive = false;
}

void APoolPawn::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ENetRole::ROLE_Authority && GetRemoteRole() == ENetRole::ROLE_SimulatedProxy ||
		GetLocalRole() == ENetRole::ROLE_AutonomousProxy && GetRemoteRole() == ENetRole::ROLE_Authority)
	{
		UE_LOG(LogTemp, Warning, TEXT("PoolPawn: %s Start preparing..."), *GetName())

		FTimerHandle timerHandle;
		GetWorld()->GetTimerManager().SetTimer(timerHandle, this, &APoolPawn::SetAsPrepared, 1);
	}
}

void APoolPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector movementInput = ConsumeMovementInputVector();
	if (!movementInput.IsNearlyZero())
	{
		FVector delta = movementInput * MaxSpeed * DeltaTime;
		AddActorWorldOffset(delta);
	}

	float yawInput = ConsumeYawInput();
	if (!FMath::IsNearlyZero(yawInput))
	{
		AddActorWorldRotation(FRotator(0, yawInput, 0));
	}
}

void APoolPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &APoolPawn::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APoolPawn::MoveRight);
	PlayerInputComponent->BindAction("Fire", EInputEvent::IE_Pressed, this, &APoolPawn::StartFire);
	PlayerInputComponent->BindAction("Fire", EInputEvent::IE_Released, this, &APoolPawn::StopFire);
	PlayerInputComponent->BindAction("Skip", EInputEvent::IE_Pressed, this, &APoolPawn::Server_Skip);
}

void APoolPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APoolPawn, bIsPrepared);
	DOREPLIFETIME(APoolPawn, bIsActive);
}

void APoolPawn::OnRep_IsPrepared() { if (bIsPrepared) ActionRouter::Server_OnPlayerPrepared.ExecuteIfBound(); }

void APoolPawn::OnRep_IsActive() {}

void APoolPawn::MoveForward(float Val)
{
	if (FMath::IsNearlyZero(Val) || !bIsActive) return;

	Server_AddMovementInput(GetControlRotation().Vector() * Val);
}

void APoolPawn::MoveRight(float Val)
{
	if (FMath::IsNearlyZero(Val) || !bIsActive) return;

	AddControllerYawInput(Val);
	Server_AddControllerYawInput(Val * GetController<APlayerController>()->InputYawScale);
}

void APoolPawn::StartFire()
{
	if (!bIsActive) return;


}

void APoolPawn::StopFire()
{
	if (!bIsActive) return;


}

void APoolPawn::Server_Skip_Implementation() { ActionRouter::Server_OnStartNextTurn.ExecuteIfBound(); }