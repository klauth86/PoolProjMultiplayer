// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/StaticMeshActor.h"
#include "BallActor.generated.h"

UCLASS()
class POOLPROJ_API ABallActor : public AStaticMeshActor
{
	GENERATED_BODY()

public:

	ABallActor();

	virtual void BeginPlay() override;

	void Wake() const;

	void Sleep() const;

	bool CanBeStopped() const;

	bool IsShot() const { return bIsShot; }

	void SetAsShot() { bIsShot = true; }

protected:

	uint8 bIsShot : 1;
};