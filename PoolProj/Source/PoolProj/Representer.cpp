// Fill out your copyright notice in the Description page of Project Settings.


#include "Representer.h"
#include "Components/StaticMeshComponent.h"

ARepresenter::ARepresenter()
{
	bReplicates = true;
	SetReplicateMovement(true);

	SetMobility(EComponentMobility::Movable);
}

void ARepresenter::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		GetStaticMeshComponent()->SetUseCCD(true);
		GetStaticMeshComponent()->SetCollisionProfileName(TEXT("BlockAll"));
		GetStaticMeshComponent()->SetNotifyRigidBodyCollision(true);
	}
}

void ARepresenter::Launch(float strength)
{
	GetStaticMeshComponent()->SetSimulatePhysics(true);
	GetStaticMeshComponent()->AddImpulse(GetActorForwardVector() * strength * 76000, NAME_None, true);
}

void ARepresenter::StartBraking()
{
	GetStaticMeshComponent()->SetAngularDamping(0.5f);
	GetStaticMeshComponent()->SetLinearDamping(0.5f);
}

void ARepresenter::StopBraking()
{
	GetStaticMeshComponent()->SetAngularDamping(0.5f);
	GetStaticMeshComponent()->SetLinearDamping(0.5f);
}

bool ARepresenter::IsStopped() const
{
	return false;
}