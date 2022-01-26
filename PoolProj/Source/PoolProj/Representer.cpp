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
	GetStaticMeshComponent()->AddImpulse(GetActorForwardVector() * strength * 7600, NAME_None, true);
}

void ARepresenter::Brake()
{

}

bool ARepresenter::IsStopped() const
{
	return true;
}