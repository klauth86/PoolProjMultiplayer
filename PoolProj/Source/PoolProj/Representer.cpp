// Fill out your copyright notice in the Description page of Project Settings.


#include "Representer.h"
#include "DecoratorActor.h"

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
		GetStaticMeshComponent()->SetGenerateOverlapEvents(true);
		GetStaticMeshComponent()->SetUseCCD(true);
		GetStaticMeshComponent()->SetCollisionProfileName(TEXT("BlockAll"));
		GetStaticMeshComponent()->SetNotifyRigidBodyCollision(true);
	}
}

void ARepresenter::Launch(float strength)
{
	GetStaticMeshComponent()->SetSimulatePhysics(true);
	GetStaticMeshComponent()->AddImpulse(GetActorForwardVector() * strength * 267600, NAME_None, true);
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

void ARepresenter::ActivateDecorator()
{
	if (DecoratorActorClass)
	{
		FVector offset = DecoratorActorClass->GetDefaultObject<ADecoratorActor>()->GetDecoratorOffset(GetWorld()->TimeSeconds);
		DecoratorActor = GetWorld()->SpawnActor<ADecoratorActor>(DecoratorActorClass, GetActorLocation() + offset, FRotator::ZeroRotator);
		DecoratorActor->SetTarget(this);
	}
}

void ARepresenter::DeActivateDecorator() {  
	if (DecoratorActor) DecoratorActor->Destroy();
	DecoratorActor = nullptr;
}