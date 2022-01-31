// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/StaticMeshActor.h"
#include "DecoratorActor.generated.h"

UCLASS()
class POOLPROJ_API ADecoratorActor : public AStaticMeshActor
{
	GENERATED_BODY()

public:

	ADecoratorActor();

public:

	virtual void Tick(float DeltaTime) override;

	void SetTarget(AActor* target) { Target = target; }

	FVector GetDecoratorOffset(float timeSeconds) const;

protected:

	UPROPERTY()
		AActor* Target;

	UPROPERTY(EditAnywhere, Category = "DecoratorActor")
		float Offset;

	UPROPERTY(EditAnywhere, Category = "DecoratorActor")
		float Amplitude;

	UPROPERTY(EditAnywhere, Category = "DecoratorActor")
		float Frequency;
};