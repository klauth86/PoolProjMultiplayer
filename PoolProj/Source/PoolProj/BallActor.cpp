// Fill out your copyright notice in the Description page of Project Settings.

#include "BallActor.h"
#include "Components/StaticMeshComponent.h"

ABallActor::ABallActor()
{
	bReplicates = true;
	SetReplicateMovement(true);

	SetMobility(EComponentMobility::Movable);

	bIsShot = false;
}

void ABallActor::BeginPlay()
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

void ABallActor::Wake() const { GetStaticMeshComponent()->SetSimulatePhysics(true); }

void ABallActor::Sleep() const { GetStaticMeshComponent()->SetSimulatePhysics(false); }

bool ABallActor::CanBeStopped() const
{
	return GetStaticMeshComponent()->GetPhysicsAngularVelocityInRadians().IsNearlyZero() &&
		GetStaticMeshComponent()->GetPhysicsLinearVelocity().IsNearlyZero();
}