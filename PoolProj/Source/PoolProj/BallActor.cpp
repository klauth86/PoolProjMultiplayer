// Fill out your copyright notice in the Description page of Project Settings.

#include "BallActor.h"
#include "Components/StaticMeshComponent.h"

ABallActor::ABallActor()
{
	bReplicates = true;
	SetReplicateMovement(true);

	SetMobility(EComponentMobility::Movable);
}

void ABallActor::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		GetStaticMeshComponent()->SetUseCCD(true);
		GetStaticMeshComponent()->SetCollisionProfileName(TEXT("BlockAll"));
		GetStaticMeshComponent()->SetNotifyRigidBodyCollision(true);
	}
}

bool ABallActor::IsStopped() const {
	if (!GetStaticMeshComponent()->IsSimulatingPhysics()) return true;

	bool nearlyNoAngularVelocity = GetStaticMeshComponent()->GetPhysicsAngularVelocity().IsNearlyZero();
	bool nearlyNoLinearVelocity = GetStaticMeshComponent()->GetPhysicsLinearVelocity().IsNearlyZero();

	if (nearlyNoAngularVelocity && nearlyNoLinearVelocity)
	{
		GetStaticMeshComponent()->PutAllRigidBodiesToSleep();
		GetStaticMeshComponent()->WakeAllRigidBodies();

		GetStaticMeshComponent()->SetSimulatePhysics(false);
		return true;
	}

	return false;
}