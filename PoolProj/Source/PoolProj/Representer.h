// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/StaticMeshActor.h"
#include "Representer.generated.h"

UCLASS()
class POOLPROJ_API ARepresenter : public AStaticMeshActor
{
	GENERATED_BODY()

public:

	ARepresenter();

	virtual void BeginPlay() override;

	void Launch(float strength);

	void StartBraking();

	void StopBraking();

	bool IsStopped() const;

protected:

	float InitAngularDamping;

	float InitLinearDamping;
};