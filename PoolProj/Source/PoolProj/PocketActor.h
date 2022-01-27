// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "PocketActor.generated.h"

UCLASS()
class POOLPROJ_API APocketActor : public AActor
{
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
};