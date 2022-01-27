// Fill out your copyright notice in the Description page of Project Settings.


#include "Representer.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BillboardComponent.h"

ARepresenter::ARepresenter()
{
	BillboardComponent = CreateDefaultSubobject<UBillboardComponent>("BillboardComponent");
	BillboardComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	BillboardComponent->SetVisibility(false);
	BillboardComponent->SetHiddenInGame(false);

	bReplicates = true;
	SetReplicateMovement(true);

	SetMobility(EComponentMobility::Movable);

	Offset = 10;
	Amplitude = 4;
	Frequency = 3;

	PrimaryActorTick.bCanEverTick = true;
}

void ARepresenter::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		GetStaticMeshComponent()->SetGenerateOverlapEvents(true);
		GetStaticMeshComponent()->SetUseCCD(true);
		GetStaticMeshComponent()->SetCollisionProfileName(TEXT("BlockAll"));
		GetStaticMeshComponent()->SetNotifyRigidBodyCollision(true);
	}
}

void ARepresenter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!BillboardComponent->IsVisible()) return;

	BillboardComponent->SetRelativeLocation(FVector(0, 0, FMath::Sin(GetWorld()->TimeSeconds * Frequency) * Amplitude + Offset));
}

void ARepresenter::Launch(float strength)
{
	GetStaticMeshComponent()->SetSimulatePhysics(true);
	GetStaticMeshComponent()->AddImpulse(GetActorForwardVector() * strength * 76000, NAME_None, true);
}

void ARepresenter::StartBraking()
{
	InitAngularDamping = GetStaticMeshComponent()->GetAngularDamping();
	InitLinearDamping = GetStaticMeshComponent()->GetLinearDamping();

	GetStaticMeshComponent()->SetAngularDamping(0.5f);
	GetStaticMeshComponent()->SetLinearDamping(0.5f);
}

void ARepresenter::StopBraking()
{
	GetStaticMeshComponent()->SetAngularDamping(InitAngularDamping);
	GetStaticMeshComponent()->SetLinearDamping(InitLinearDamping);
}

void ARepresenter::Stop() const
{
	GetStaticMeshComponent()->PutAllRigidBodiesToSleep();
	GetStaticMeshComponent()->WakeAllRigidBodies();

	GetStaticMeshComponent()->SetSimulatePhysics(false);
}

bool ARepresenter::IsStopped() const
{
	if (!GetStaticMeshComponent()->IsSimulatingPhysics()) return true;

	bool nearlyNoAngularVelocity = GetStaticMeshComponent()->GetPhysicsAngularVelocityInRadians().IsNearlyZero();
	bool nearlyNoLinearVelocity = GetStaticMeshComponent()->GetPhysicsLinearVelocity().IsNearlyZero();
	
	if (nearlyNoAngularVelocity && nearlyNoLinearVelocity)
	{
		Stop();
		return true;
	}

	return false;
}

void ARepresenter::ActivateDecor() const
{
	BillboardComponent->SetRelativeLocation(FVector(0, 0, FMath::Sin(GetWorld()->TimeSeconds * Frequency) * Amplitude + Offset));
	BillboardComponent->SetVisibility(true);
}

void ARepresenter::DeactivateDecor() const { BillboardComponent->SetVisibility(false); }