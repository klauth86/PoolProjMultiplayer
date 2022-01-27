// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/StaticMeshActor.h"
#include "Representer.generated.h"

class UDecoratorWidget;
class UBillboardComponent;

UCLASS()
class POOLPROJ_API ARepresenter : public AStaticMeshActor
{
	GENERATED_BODY()

public:

	ARepresenter();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	void Launch(float strength);

	void StartBraking();

	void StopBraking();

	void Stop() const;

	bool IsStopped() const;

	void ActivateDecor() const;

	void DeactivateDecor() const;

protected:

	float InitAngularDamping;

	float InitLinearDamping;

	UPROPERTY(EditAnywhere, Category = "Representer")
		float Offset;

	UPROPERTY(EditAnywhere, Category = "Representer")
		float Amplitude;

	UPROPERTY(EditAnywhere, Category = "Representer")
		float Frequency;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Representer")
		UBillboardComponent* BillboardComponent;
};