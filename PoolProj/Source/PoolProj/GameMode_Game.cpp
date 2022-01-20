// Fill out your copyright notice in the Description page of Project Settings.

#include "GameMode_Game.h"
#include "PoolPawn.h"
#include "EngineUtils.h"
#include "Common.h"
#include "ActionRouter.h"

AGameMode_Game::AGameMode_Game()
{
	ResetScore();
}

void AGameMode_Game::RestartPlayer(AController* NewPlayer)
{
	if (NewPlayer == nullptr || NewPlayer->IsPendingKillPending()) return;

	UWorld* World = GetWorld();

	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* actor = *It;
		if (actor->ActorHasTag(FName(NewPlayer->IsLocalPlayerController() ? PPTags::Player1Portal : PPTags::Player2Portal)))
		{
			return RestartPlayerAtPlayerStart(NewPlayer, actor);
		}
	}
}

void AGameMode_Game::BeginPlay()
{
	ActionRouter::Server_OnPlayerPrepared.BindUObject(this, &AGameMode_Game::OnPlayerPrepared);
	Super::BeginPlay();
}

void AGameMode_Game::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ActionRouter::Server_OnPlayerPrepared.Unbind();
	Super::EndPlay(EndPlayReason);
}

void AGameMode_Game::OnPlayerPrepared()
{
	for (TActorIterator<APoolPawn> It(GetWorld()); It; ++It)
	{
		APoolPawn* pawn = *It;
		if (pawn->IsPreparing()) return;
	}

	StartNextTurn();
}

void AGameMode_Game::StartNextTurn()
{
	int32 activePawnIndex = INDEX_NONE;
	TArray<APoolPawn*> pawns;
	
	for (TActorIterator<APoolPawn> It(GetWorld()); It; ++It)
	{
		APoolPawn* pawn = *It;
		int32 pawnIndex = pawns.Add(pawn);
		if (pawn->IsActive()) activePawnIndex = pawnIndex;
	}

	pawns[activePawnIndex]->SetIsActive(false);
	activePawnIndex = (activePawnIndex + 1) % pawns.Num();
	pawns[activePawnIndex]->SetIsActive(true);
}

void AGameMode_Game::PostLogin(APlayerController* NewPlayer) {
	Super::PostLogin(NewPlayer);

	UE_LOG(LogTemp, Warning, TEXT("@@@ %s"), NewPlayer->GetPawn() ? *NewPlayer->GetPawn()->GetName() : TEXT("NULL"))
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