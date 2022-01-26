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

	MaxSpeed = 1200;
	TargetLength = 30;
	TargetAngle = 30;
	bIsPrepared = false;
	bIsActive = false;
}

void APoolPawn::BeginPlay()
{
	Super::BeginPlay();

	UWorld* world = GetWorld();

	if (HasNetOwner())
	{
		UE_LOG(LogTemp, Warning, TEXT("*** %s: %s Start preparing..."), *(world->GetNetMode() == ENetMode::NM_Client ? FString::Printf(TEXT("Client %d"), GPlayInEditorID) : FString("Server")), *GetName());

		FTimerHandle timerHandle;
		GetWorld()->GetTimerManager().SetTimer(timerHandle, this, &APoolPawn::Server_SetAsPrepared, 1);
	}
}

void APoolPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		FVector movementInput = ConsumeMovementInputVector();
		if (!movementInput.IsNearlyZero())
		{
			FVector delta = movementInput * MaxSpeed * DeltaTime;
			if (Representer) Representer->AddActorWorldOffset(delta);
		}

		float yawInput = ConsumeYawInput();
		if (!FMath::IsNearlyZero(yawInput))
		{
			if (Representer) Representer->AddActorWorldRotation(FRotator(0, yawInput, 0));
		}
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
	DOREPLIFETIME(APoolPawn, Representer);
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