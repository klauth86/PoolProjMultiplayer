// Fill out your copyright notice in the Description page of Project Settings.

#include "PoolPawn.h"
#include "Camera/CameraComponent.h"
#include "Net/UnrealNetwork.h"
#include "EngineUtils.h"
#include "Common.h"
#include "ActionRouter.h"
#include "Representer.h"
#include "BallActor.h"
#include "UI/GameWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameDataSingleton.h"

APoolPawn::APoolPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");

	RespawnDistance = 500;

	MaxSpeed = 400;
	TargetLength = 30;
	TargetAngle = 30;

	RestorePositionTime = 0.5f;

	YawInput = 0;

	StrengthTime = 4;
	Strength = 0;

	bIsPrepared = false;
	bIsActive = false;

	bIsFloatingToRepresenter = false;
	bHasBeenLaunched = false;
	bIsActionPressed = false;
	bIsFloatingToRepresenter_Client = false;
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
				AttachToActor(Representer, FAttachmentTransformRules::KeepWorldTransform);
				break;
			}
		}

		ActionRouter::Server_OnShot.AddUObject(this, &APoolPawn::OnShot);
	}
	
	if (HasNetOwner()) { 
		InitUI();
		ActionRouter::Client_OnPrepared.AddUObject(this, &APoolPawn::Client_OnPrepared);
	}
}

void APoolPawn::EndPlay(EEndPlayReason::Type endPlayReason)
{
	ActionRouter::Client_OnPrepared.RemoveAll(this);

	ActionRouter::Server_OnShot.RemoveAll(this);

	Super::EndPlay(endPlayReason);
}

void APoolPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsActive) {
		FRotator targetRotation = (Representer->GetActorLocation() - GetActorLocation()).Rotation();
		if (!(targetRotation - GetActorRotation()).IsNearlyZero()) {
			SetActorRotation(targetRotation);
		}
	}
	if (bHasBeenLaunched && !bIsFloatingToRepresenter)
	{
		SetActorRotation((Representer->GetActorLocation() - GetActorLocation()).Rotation());
	}
	else if (bIsFloatingToRepresenter)
	{
		if (!bIsFloatingToRepresenter_Client)
		{
			PreFloatLocation = GetActorLocation();
			PreFloatRotation = GetActorRotation();

			bIsFloatingToRepresenter_Client = true;
		}
		else {
			if (RestorePositionTimeLeft > 0) {
				float alpha = 1 - RestorePositionTimeLeft / RestorePositionTime;
				SetActorLocation(FMath::Lerp(PreFloatLocation, Representer->GetActorLocation() + GetRepresenterOffset(Representer->GetActorRotation()), alpha));
				SetActorRotation((Representer->GetActorLocation() - GetActorLocation()).Rotation());
			}
			else {
				SetActorLocation(Representer->GetActorLocation() + GetRepresenterOffset(Representer->GetActorRotation()));
				SetActorRotation((Representer->GetActorLocation() - GetActorLocation()).Rotation());
				
				bIsFloatingToRepresenter_Client = false;
			}
		}
	}
	
	if (!HasAuthority()) return;

	if (bIsActive && Representer->GetActorLocation().SizeSquared() > RespawnDistance * RespawnDistance)
	{
		Representer->Stop();

		Representer->SetActorLocation(PreLaunchLocation); ////// TODO Check if there could be collision with ball!
		Representer->SetActorRotation(PreLaunchRotation);

		AttachToActor(Representer, FAttachmentTransformRules::KeepWorldTransform);

		Client_OnLostPoint();

		bool allBallsAreStopped = true;

		for (TActorIterator<ABallActor> It(GetWorld()); It; ++It)
		{
			ABallActor* ball = *It;
			if (ball->IsShot()) continue;
			if (!ball->CanBeStopped()) allBallsAreStopped = false;
		}

		if (allBallsAreStopped) EndTurn();
	}
	else if (bIsActionPressed && !bHasBeenLaunched)
	{
		StrengthTimeLeft -= DeltaTime;
		if (StrengthTimeLeft < 0) StrengthTimeLeft = 0;
		SetStrength(FMath::Lerp(1.f, 0.f, StrengthTimeLeft / StrengthTime));

		bIsActionPressedLastFrame = bIsActionPressed;
	}
	else if (bIsActionPressedLastFrame && !bHasBeenLaunched)
	{
		bIsActionPressedLastFrame = false;

		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		PreLaunchLocation = Representer->GetActorLocation();
		PreLaunchRotation = Representer->GetActorRotation();

		WakeUpBalls();

		Representer->Launch(Strength);
		bHasBeenLaunched = true;
	}
	else if (bHasBeenLaunched && !bIsFloatingToRepresenter)
	{
		if (bIsActionPressed && !bIsActionPressedLastFrame) Representer->StartBraking();

		if (!bIsActionPressed && bIsActionPressedLastFrame) Representer->StopBraking();

		if (Representer->IsStopped())
		{
			bool allBallsAreStopped = true;

			for (TActorIterator<ABallActor> It(GetWorld()); It; ++It)
			{
				ABallActor* ball = *It;
				if (ball->IsShot()) continue;
				if (!ball->CanBeStopped()) allBallsAreStopped = false;
			}

			if (allBallsAreStopped)
			{
				PreLaunchRotation = Representer->GetActorRotation(); // Use same variable avoiding of need to add other
				RestorePositionTimeLeft = RestorePositionTime;				
				bIsFloatingToRepresenter = true;
				
				EndTurn();
			}
		}
	}
	else if (bIsFloatingToRepresenter)
	{
		RestorePositionTimeLeft -= DeltaTime;

		FRotator targetRotation = FRotator(0, 0, PreLaunchRotation.Yaw);

		if (RestorePositionTimeLeft > 0)
		{
			float alpha = 1 - RestorePositionTimeLeft / RestorePositionTime;
			Representer->SetActorRotation(FMath::Lerp(PreLaunchRotation, targetRotation, alpha));
		}
		else
		{
			Representer->SetActorRotation(targetRotation);
					
			AttachToActor(Representer, FAttachmentTransformRules::KeepWorldTransform);

			bIsFloatingToRepresenter = false;
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
	DOREPLIFETIME(APoolPawn, RestorePositionTimeLeft);
	DOREPLIFETIME(APoolPawn, Shots);
	DOREPLIFETIME(APoolPawn, Strength);
	DOREPLIFETIME(APoolPawn, Representer);
	DOREPLIFETIME(APoolPawn, bIsPrepared);
	DOREPLIFETIME(APoolPawn, bIsActive);
	DOREPLIFETIME(APoolPawn, bIsFloatingToRepresenter);
	DOREPLIFETIME(APoolPawn, bHasBeenLaunched);
}

void APoolPawn::SetStrength(float strength)
{
	Strength = strength;

	if (GameWidget)
	{
		GameWidget->OnStrength(Strength);
	}
}

void APoolPawn::SetIsActive(bool isActive)
{
	bIsActive = isActive;
	
	if (GameWidget)
	{
		if (bIsActive) 
			GameWidget->OnStartTurn();
		else
			GameWidget->OnOpponentTurn();
	}

	if (GetRemoteRole() == ENetRole::ROLE_AutonomousProxy && Representer) return bIsActive ? Representer->ActivateDecorator() : Representer->DeActivateDecorator();
}

FVector APoolPawn::GetRepresenterOffset(FRotator rotation) const
{
	static float cos = FMath::Cos(TargetAngle / 180 * PI);
	static float sin = FMath::Sin(TargetAngle / 180 * PI);
	return -rotation.Vector() * TargetLength * cos + FVector(0, 0, TargetLength * sin);

	return FVector::ZeroVector;
}

void APoolPawn::MoveForward(float Val)
{
	if (FMath::IsNearlyZero(Val) || !bIsActive) return;

	Server_AddMovementInput(Representer->GetActorForwardVector() * Val);
}

void APoolPawn::MoveRight(float Val)
{
	if (FMath::IsNearlyZero(Val)) return;

	Server_AddControllerYawInput(Val * GetController<APlayerController>()->InputYawScale);
}

void APoolPawn::Server_StartFire_Implementation()
{
	if (!bIsActive) return;

	if (!bHasBeenLaunched) StrengthTimeLeft = StrengthTime;
	
	bIsActionPressed = true;
}

void APoolPawn::Server_StopFire_Implementation()
{
	if (!bIsActive) return;

	bIsActionPressed = false;
}

void APoolPawn::Server_Skip_Implementation() { if (!bIsActive) return;  ActionRouter::Server_OnStartNextTurn.ExecuteIfBound(); }

void APoolPawn::OnRep_Strength()
{
	if (GameWidget)
	{
		GameWidget->OnStrength(Strength);
	}
}

void APoolPawn::OnRep_IsPrepared() { if (bIsPrepared) ActionRouter::Server_OnPlayerPrepared.ExecuteIfBound(); }

void APoolPawn::OnRep_IsActive()
{
	if (GameWidget)
	{
		if (bIsActive)
			GameWidget->OnStartTurn();
		else
			GameWidget->OnOpponentTurn();
	}

	if (!HasNetOwner() && Representer) return bIsActive ? Representer->ActivateDecorator() : Representer->DeActivateDecorator();
}

void APoolPawn::OnRep_Shots()
{
	if (HasNetOwner())
	{

	}
}

void APoolPawn::Client_OnPrepared(UObject* widgetOwner) { if (widgetOwner == GetController()) Server_SetAsPrepared(); }

void APoolPawn::InitUI()
{
	if (GameWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("*** %s: %s InitUI!"), *(GetWorld()->GetNetMode() == ENetMode::NM_Client ? FString::Printf(TEXT("Client %d"), GPlayInEditorID) : FString("Server")), *GetName());

		if (UGameWidget* gameWidget = CreateWidget<UGameWidget>(GetController<APlayerController>(), GameWidgetClass))
		{
			GameWidget = gameWidget;
			GameWidget->AddToViewport();
			GameWidget->OnOpponentTurn();
		}
	}
}

void APoolPawn::UnInitUI()
{
	if (GameWidget) GameWidget->RemoveFromViewport();
	GameWidget = nullptr;
}

void APoolPawn::EndTurn()
{
	PutBallsToSleep();

	bHasBeenLaunched = false;
	bIsActionPressed = false;

	SetStrength(0);

	Server_Skip_Implementation();
}

void APoolPawn::Client_OnLostPoint_Implementation() {
	UE_LOG(LogTemp, Warning, TEXT("*** %s: %s Client_OnLostPoint!"), *(GetWorld()->GetNetMode() == ENetMode::NM_Client ? FString::Printf(TEXT("Client %d"), GPlayInEditorID) : FString("Server")), *Representer->GetName());

	if (USoundBase* soundBase = UGameDataSingleton::GetInstance()->GetLostPointSfx())
	{
		UGameplayStatics::PlaySound2D(GetWorld(), soundBase);
	}
}

void APoolPawn::PutBallsToSleep()
{
	for (TActorIterator<ABallActor> It(GetWorld()); It; ++It)
	{
		ABallActor* ballActor = *It;
		ballActor->Sleep();
	}
}

void APoolPawn::WakeUpBalls()
{
	for (TActorIterator<ABallActor> It(GetWorld()); It; ++It)
	{
		ABallActor* ballActor = *It;
		ballActor->Wake();
	}
}