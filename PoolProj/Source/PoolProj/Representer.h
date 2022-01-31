// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/StaticMeshActor.h"
#include "Representer.generated.h"

class ADecoratorActor;

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

	void Stop() const;

	bool IsStopped() const;

	void ActivateDecorator();

	void DeActivateDecorator();

protected:

	float InitAngularDamping;

	float InitLinearDamping;

	UPROPERTY(EditDefaultsOnly, Category = "Representer")
		TSubclassOf<ADecoratorActor> DecoratorActorClass;

	UPROPERTY()
		ADecoratorActor* DecoratorActor;
};