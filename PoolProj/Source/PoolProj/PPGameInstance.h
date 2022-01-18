// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "PPGameInstance.generated.h"

class UBaseWidget;
class UMenuWidget;
class ULobbyWidget;
class UGameWidget;

UCLASS()
class POOLPROJ_API UPPGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	virtual void Init() override;
	virtual void BeginDestroy() override;

	virtual void OnWorldChanged(UWorld* OldWorld, UWorld* NewWorld) override;

	void CreateGame();
	void DestroyGame();
	void JoinGame(int32 Index);

	void StartGame();

protected:

	bool IsLevel(const FSoftObjectPath& softPath) const;
	FString GetAssetPathFromSoftPath(const FSoftObjectPath& softPath) const;

	void OnWorldBeginPlay();
	void ClearUI();

	bool IsMenu() const { return IsLevel(MenuLevel); }
	bool IsLobby() const { return IsLevel(LobbyLevel); }
	bool IsGame() const { return IsLevel(GameLevel); }

	void CreateSession();
	void OnCreateSessionComplete(FName SessionName, bool Success);
	void OnDestroySessionComplete(FName SessionName, bool Success);
	void OnFindSessionsComplete(bool Success);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString) { ClientGoTo(MenuLevel); }

	void OnUIConstruct();
	void OnUIDestruct();

	void ClientGoTo(FString address);
	void ClientGoTo(const FSoftObjectPath& softPath) { ClientGoTo(GetAssetPathFromSoftPath(softPath)); }
	void ServerGoTo(const FSoftObjectPath& softPath);

	void OnRefresh();

protected:

	UPROPERTY()
		UBaseWidget* CurrentWidget;

	UPROPERTY(EditDefaultsOnly, Category = "PPGameInstance: UI")
		TSubclassOf<UMenuWidget> MenuWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "PPGameInstance: UI")
		TSubclassOf<ULobbyWidget> LobbyWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "PPGameInstance: UI")
		TSubclassOf<UGameWidget> GameWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "PPGameInstance: Levels", meta = (AllowedClasses = "World"))
		FSoftObjectPath MenuLevel;

	UPROPERTY(EditDefaultsOnly, Category = "PPGameInstance: Levels", meta = (AllowedClasses = "World"))
		FSoftObjectPath LobbyLevel;

	UPROPERTY(EditDefaultsOnly, Category = "PPGameInstance: Levels", meta = (AllowedClasses = "World"))
		FSoftObjectPath GameLevel;

	uint8 bCreateNewSession : 1;

	uint8 bIsRefreshing : 1;
};