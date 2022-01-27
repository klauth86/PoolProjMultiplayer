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

void ABallActor::Prepare() const
{
	GetStaticMeshComponent()->SetSimulatePhysics(true);
}

void ABallActor::Stop() const
{
	GetStaticMeshComponent()->PutAllRigidBodiesToSleep();
	GetStaticMeshComponent()->WakeAllRigidBodies();

	GetStaticMeshComponent()->SetSimulatePhysics(false);
}

bool ABallActor::IsStopped() const
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