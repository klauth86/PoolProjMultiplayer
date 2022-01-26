// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LaunchableActor.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class ULaunchableActor : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class POOLPROJ_API ILaunchableActor
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual void Launch(float strength) {}

	virtual void StartBraking() {}

	virtual void StopBraking() {}

	virtual bool IsStopped() const { return true; }
};
