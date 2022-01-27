// Fill out your copyright notice in the Description page of Project Settings.

#include "GameMode_Game.h"
#include "PoolPawn.h"
#include "EngineUtils.h"
#include "Common.h"
#include "ActionRouter.h"

AGameMode_Game::AGameMode_Game()
{
}

void AGameMode_Game::RestartPlayer(AController* NewPlayer)
{
	if (NewPlayer == nullptr || NewPlayer->IsPendingKillPending()) return;

	UWorld* World = GetWorld();

	FConstPlayerControllerIterator ControllerIt = World->GetPlayerControllerIterator();
	while (ControllerIt && *ControllerIt != NewPlayer) ControllerIt++;;

	if (ControllerIt)
	{
		FName playerTag = FName(PPTags::GetPlayerTag(ControllerIt.GetIndex()));
		for (TActorIterator<AActor> It(World); It; ++It)
		{
			AActor* actor = *It;
			if (actor->ActorHasTag(playerTag))
			{
				UClass* pawnClass = GetDefaultPawnClassForController(NewPlayer);

				float targetLength = pawnClass->GetDefaultObject<APoolPawn>()->GetTargetLength();
				float targetAngle = pawnClass->GetDefaultObject<APoolPawn>()->GetTargetAngle();

				FTransform transform;
				transform.SetLocation(actor->GetActorLocation()
					- actor->GetActorForwardVector() * targetLength * FMath::Cos(targetAngle / 180 * PI) + FVector(0, 0, targetLength * FMath::Sin(targetAngle / 180 * PI)));
				transform.SetRotation((actor->GetActorLocation() - transform.GetLocation()).ToOrientationQuat());
				
				return RestartPlayerAtTransform(NewPlayer, transform);
			}
		}
	}
}

void AGameMode_Game::BeginPlay()
{
	ActionRouter::Server_OnPlayerPrepared.BindUObject(this, &AGameMode_Game::OnPlayerPrepared);
	ActionRouter::Server_OnStartNextTurn.BindUObject(this, &AGameMode_Game::StartNextTurn);
	
	Super::BeginPlay();
}

void AGameMode_Game::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ActionRouter::Server_OnPlayerPrepared.Unbind();
	ActionRouter::Server_OnStartNextTurn.Unbind();

	Super::EndPlay(EndPlayReason);
}

void AGameMode_Game::OnPlayerPrepared()
{
	for (TActorIterator<APoolPawn> It(GetWorld()); It; ++It)
	{
		APoolPawn* pawn = *It;
		if (!pawn->IsPrepared()) return;
	}

	StartNextTurn();
}

void AGameMode_Game::StartNextTurn()
{
	int32 activePawnIndex = INDEX_NONE;
	TArray<APoolPawn*> pawns;
	
	UWorld* world = GetWorld();

	for (TActorIterator<APoolPawn> It(world); It; ++It)
	{
		APoolPawn* pawn = *It;
		int32 pawnIndex = pawns.Add(pawn);
		if (pawn->IsActive()) activePawnIndex = pawnIndex;
	}

	if (pawns.IsValidIndex(activePawnIndex)) pawns[activePawnIndex]->SetIsActive(false);
	
	activePawnIndex = (activePawnIndex + 1) % pawns.Num();
	
	pawns[activePawnIndex]->SetIsActive(true);

	UE_LOG(LogTemp, Warning, TEXT("*** %s: StartNextTurn... %s is Playing!"), *(world->GetNetMode() == ENetMode::NM_Client ? FString::Printf(TEXT("Client %d"), GPlayInEditorID) : FString("Server")), *pawns[activePawnIndex]->GetName());
}