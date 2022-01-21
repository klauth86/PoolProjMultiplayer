// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameModeBase.h"
#include "GameMode_Game.generated.h"

UCLASS()
class POOLPROJ_API AGameMode_Game : public AGameModeBase
{
	GENERATED_BODY()

public:

	AGameMode_Game();

	virtual void RestartPlayer(AController* NewPlayer) override;

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:

	void OnPlayerPrepared();

	void StartNextTurn();
};