// Fill out your copyright notice in the Description page of Project Settings.

#include "GameMode_Game.h"
#include "MyPawn.h"

AGameMode_Game::AGameMode_Game()
{
	DefaultPawnClass = AMyPawn::StaticClass();
	ResetScore();
}

void AGameMode_Game::CheckWinCondition(int ballCount)
{
	if (ballCount == 0)
	{
		ActiveControllerId = -1;

		if (Player1Score > Player2Score)
		{
			Winner = FName("Player 1");
		}
		else
		{
			Winner = FName("Player 2");
		}

		UE_LOG(LogTemp, Warning, TEXT("CheckWinCondition %d"), ballCount);
	}
}

void AGameMode_Game::ResetScore()
{
	Player1Score = 0;
	Player2Score = 0;
	ActiveControllerId = 0;
}