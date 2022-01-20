// Fill out your copyright notice in the Description page of Project Settings.

#include "PoolPawn.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Net/UnrealNetwork.h"
#include "ActionRouter.h"

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
	bIsPreparing = false;
	bIsActive = false;
}

void APoolPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

void APoolPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		FVector movementInput = ConsumeMovementInputVector();
		
		AddActorWorldOffset(movementInput * MaxSpeed * DeltaTime, true);

		UE_LOG(LogTemp, Warning, TEXT("Tick"))
	}
}

void APoolPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &APoolPawn::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APoolPawn::MoveRight);
	PlayerInputComponent->BindAction("Fire", EInputEvent::IE_Pressed, this, &APoolPawn::StartFire);
	PlayerInputComponent->BindAction("Fire", EInputEvent::IE_Released, this, &APoolPawn::StopFire);
}

void APoolPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APoolPawn, bIsPreparing);
	DOREPLIFETIME(APoolPawn, bIsActive);
}

void APoolPawn::OnRep_IsPreparing() { if (!bIsPreparing) ActionRouter::Server_OnPlayerPrepared.ExecuteIfBound(); }

void APoolPawn::MoveForward(float Val)
{
	if (FMath::IsNearlyZero(Val) || !bIsActive) return;

	Server_AddMovementInput(GetControlRotation().Vector() * Val);
}

void APoolPawn::MoveRight(float Val)
{
	if (FMath::IsNearlyZero(Val) || !bIsActive) return;

	Server_AddMovementInput(-GetControlRotation().Vector() * Val);
}

void APoolPawn::StartFire()
{
	if (!bIsActive) return;


}

void APoolPawn::StopFire()
{
	if (!bIsActive) return;


}