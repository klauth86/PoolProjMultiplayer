// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/StaticMeshActor.h"
#include "Interface/LaunchableActor.h"
#include "Representer.generated.h"

UCLASS()
class POOLPROJ_API ARepresenter : public AStaticMeshActor, public ILaunchableActor
{
	GENERATED_BODY()

public:

	ARepresenter();

	virtual void BeginPlay() override;

	virtual void Launch(float strength) override;

	virtual void StartBraking();

	virtual void StopBraking();

	virtual bool IsStopped() const override;
};