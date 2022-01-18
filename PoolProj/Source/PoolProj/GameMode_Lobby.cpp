// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameMode_Lobby.h"
#include "ActionRouter.h"

void AGameMode_Lobby::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	++NumberOfPlayers;

	if (NumberOfPlayers >= 2)
	{
		////// TODO Add progress widget (AUTHORITY) when we are almost starting

		FTimerHandle timerHandle;
		GetWorldTimerManager().SetTimer(timerHandle, this, &AGameMode_Lobby::StartGame, 1);
	}
}

void AGameMode_Lobby::Logout(AController* Exiting)
{	
	--NumberOfPlayers;

	Super::Logout(Exiting);
}

void AGameMode_Lobby::StartGame()
{
	bUseSeamlessTravel = true;
	ActionRouter::OnStartGame.ExecuteIfBound();
}