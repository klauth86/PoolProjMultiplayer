// Fill out your copyright notice in the Description page of Project Settings.

#include "PocketActor.h"
#include "BallActor.h"
#include "Representer.h"

void APocketActor::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		////// TODO SetUp Overlaps only on Authority
	}
}

void APocketActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (ARepresenter* representer = Cast<ARepresenter>(OtherActor))
	{

	}
	else if (ABallActor* ball = Cast<ABallActor>(OtherActor))
	{

	}
}