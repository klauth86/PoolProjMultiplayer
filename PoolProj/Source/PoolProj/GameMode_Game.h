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

	void CheckWinCondition(int ballCount);

	void ResetScore();

public:

	int32 Player1Score;

	int32 Player2Score;

	int32 ActiveControllerId;

	FName Winner;
};