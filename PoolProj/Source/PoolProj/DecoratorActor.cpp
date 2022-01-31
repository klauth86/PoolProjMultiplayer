// Fill out your copyright notice in the Description page of Project Settings.

#include "DecoratorActor.h"

ADecoratorActor::ADecoratorActor()
{
	GetStaticMeshComponent()->SetCollisionProfileName(TEXT("NoCollision"));

	SetMobility(EComponentMobility::Movable);

	Offset = 180;
	Amplitude = 30;
	Frequency = 8;

	PrimaryActorTick.bCanEverTick = true;
}

void ADecoratorActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!Target) return;

	SetActorLocation(Target->GetActorLocation() + GetDecoratorOffset(GetWorld()->TimeSeconds));
}

FVector ADecoratorActor::GetDecoratorOffset(float timeSeconds) const
{
	return FVector(0, 0, FMath::Sin(timeSeconds * Frequency) * Amplitude + Offset);
}