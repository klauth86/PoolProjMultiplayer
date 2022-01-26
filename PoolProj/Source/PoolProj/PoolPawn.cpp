// Fill out your copyright notice in the Description page of Project Settings.

#include "PoolPawn.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Net/UnrealNetwork.h"
#include "ActionRouter.h"
#include "TimerManager.h"
#include "EngineUtils.h"
#include "Common.h"
#include "Representer.h"
#include "BallActor.h"

APoolPawn::APoolPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	MaxSpeed = 400;
	TargetLength = 30;
	TargetAngle = 30;

	YawInput = 0;

	StrengthTime = 4;
	Strength = 0;

	bIsPrepared = false;
	bIsActive = false;
	bHasBeenLaunched = false;
	bIsActionPressed = false;
}

void APoolPawn::BeginPlay()
{
	Super::BeginPlay();

	UWorld* world = GetWorld();

	if (HasAuthority())
	{
		int32 index = world->GetFirstPlayerController() == GetController() ? 0 : 1;
		FName playerTag = FName(PPTags::GetPlayerTag(index));

		for (TActorIterator<ARepresenter> It(world); It; ++It)
		{
			ARepresenter* representer = *It;
			if (representer->ActorHasTag(playerTag))
			{
				Representer = representer;
				break;
			}
		}
	}

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
		if (bIsActionPressed && !bHasBeenLaunched)
		{
			StrengthTimeLeft -= DeltaTime;
			if (StrengthTimeLeft < 0) StrengthTimeLeft = 0;
			Strength = FMath::Lerp(1.f, 0.f, StrengthTimeLeft / StrengthTime);

			bIsActionPressedLastFrame = bIsActionPressed;
		}
		else if (bIsActionPressedLastFrame && !bHasBeenLaunched)
		{
			bIsActionPressedLastFrame = false;

			Representer->Launch(Strength);
			bHasBeenLaunched = true;
		}
		else if (bHasBeenLaunched)
		{
			if (bIsActionPressed && !bIsActionPressedLastFrame) Representer->StartBraking();
			
			if (!bIsActionPressed && bIsActionPressedLastFrame) Representer->StopBraking();

			if (Representer->IsStopped())
			{
				bool allBallsAreStopped = true;

				for (TActorIterator<ABallActor> It(world); It; ++It)
				{
					ABallActor* ballActor = *It;
					if (!ballActor->IsStopped()) allBallsAreStopped = false;
				}

				if (allBallsAreStopped)
				{
					bHasBeenLaunched = false;
					Server_Skip_Implementation();
				}
			}
		}
		else
		{
			FVector movementInput = ConsumeMovementInputVector();
			if (!movementInput.IsNearlyZero())
			{
				FVector delta = movementInput * MaxSpeed * DeltaTime;
				Representer->AddActorWorldOffset(delta, true);
			}

			float yawInput = ConsumeYawInput();
			if (!FMath::IsNearlyZero(yawInput))
			{
				Representer->AddActorWorldRotation(FRotator(0, yawInput, 0));
			}
		}
	}

	if (HasNetOwner())
	{
		if (!Representer) return;

		if (bHasBeenLaunched) {
			GetController()->SetControlRotation((Representer->GetActorLocation() - GetActorLocation()).Rotation());
		}
		else
		{
			FVector representerOffset = GetRepresenterOffset();
			if (!representerOffset.IsNearlyZero())
			{
				SetActorLocation(Representer->GetActorLocation() + representerOffset);
			}
		}
	}
}

void APoolPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &APoolPawn::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APoolPawn::MoveRight);
	PlayerInputComponent->BindAction("Fire", EInputEvent::IE_Pressed, this, &APoolPawn::Server_StartFire);
	PlayerInputComponent->BindAction("Fire", EInputEvent::IE_Released, this, &APoolPawn::Server_StopFire);
	PlayerInputComponent->BindAction("Skip", EInputEvent::IE_Pressed, this, &APoolPawn::Server_Skip);
}

void APoolPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APoolPawn, Strength);
	DOREPLIFETIME(APoolPawn, Representer);
	DOREPLIFETIME(APoolPawn, bIsPrepared);
	DOREPLIFETIME(APoolPawn, bIsActive);
	DOREPLIFETIME(APoolPawn, bHasBeenLaunched);
}

void APoolPawn::OnRep_IsPrepared() { if (bIsPrepared) ActionRouter::Server_OnPlayerPrepared.ExecuteIfBound(); }

void APoolPawn::OnRep_IsActive() {}

void APoolPawn::MoveForward(float Val)
{
	if (FMath::IsNearlyZero(Val) || !bIsActive) return;

	Server_AddMovementInput(Representer->GetActorForwardVector() * Val);
}

void APoolPawn::MoveRight(float Val)
{
	if (FMath::IsNearlyZero(Val) || !bIsActive) return;

	AddControllerYawInput(Val);
	Server_AddControllerYawInput(Val * GetController<APlayerController>()->InputYawScale);
}

void APoolPawn::Server_StartFire_Implementation()
{
	if (!bIsActive) return;

	if (!bHasBeenLaunched)
	{
		StrengthTimeLeft = StrengthTime;
		Strength = 0;

		bIsActionPressed = true;
	}
	else
	{
		bIsActionPressed = true;
	}
}

void APoolPawn::Server_StopFire_Implementation()
{
	if (!bIsActive) return;

	bIsActionPressed = false;
}

void APoolPawn::Server_Skip_Implementation() { ActionRouter::Server_OnStartNextTurn.ExecuteIfBound(); }

FVector APoolPawn::GetRepresenterOffset() const
{
	static float cos = FMath::Cos(TargetAngle / 180 * PI);
	static float sin = FMath::Sin(TargetAngle / 180 * PI);
	return -Representer->GetActorForwardVector() * TargetLength * cos + FVector(0, 0, TargetLength * sin);

	return FVector::ZeroVector;
}