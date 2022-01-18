// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/GameModeBase.h"
#include "GameMode_Lobby.generated.h"

UCLASS()
class POOLPROJ_API AGameMode_Lobby : public AGameModeBase
{
	GENERATED_BODY()

public:

	void PostLogin(APlayerController* NewPlayer) override;

	void Logout(AController* Exiting) override;

	void StartGame();

protected:

	int32 NumberOfPlayers = 0;
};