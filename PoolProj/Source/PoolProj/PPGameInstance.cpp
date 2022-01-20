// Fill out your copyright notice in the Description page of Project Settings.

#include "PPGameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "ActionRouter.h"
#include "UI/MenUWidget.h"
#include "UI/LobbyWidget.h"

const static FName SESSION_NAME = TEXT("Game");

IOnlineSessionPtr SessionInterface;
TSharedPtr<class FOnlineSessionSearch> SessionSearch;

void UPPGameInstance::Init()
{
	bCreateNewSession = 0;
	
	bIsRefreshing = 0;

	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found subsystem %s"), *Subsystem->GetSubsystemName().ToString());
		SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UPPGameInstance::OnCreateSessionComplete);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UPPGameInstance::OnDestroySessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UPPGameInstance::OnFindSessionsComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UPPGameInstance::OnJoinSessionComplete);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Found no subsystem"));
	}

	if (GEngine != nullptr)
	{
		GEngine->OnNetworkFailure().AddUObject(this, &UPPGameInstance::OnNetworkFailure);
	}

	ActionRouter::OnCreateGame.BindUObject(this, &UPPGameInstance::CreateGame);
	ActionRouter::OnDestroyGame.BindUObject(this, &UPPGameInstance::DestroyGame);
	ActionRouter::OnJoinGame.BindUObject(this, &UPPGameInstance::JoinGame);

	ActionRouter::OnStartGame.BindUObject(this, &UPPGameInstance::StartGame);

	ActionRouter::OnRefresh.BindUObject(this, &UPPGameInstance::OnRefresh);
}

void UPPGameInstance::BeginDestroy()
{
	OnWorldChanged(GetWorld(), nullptr);

	SessionInterface.Reset();
	SessionSearch.Reset();

	Super::BeginDestroy();
}

void UPPGameInstance::OnWorldChanged(UWorld* OldWorld, UWorld* NewWorld)
{
	if (OldWorld) OldWorld->OnWorldBeginPlay.RemoveAll(this);

	if (NewWorld) NewWorld->OnWorldBeginPlay.AddUObject(this, &UPPGameInstance::OnWorldBeginPlay);
}

void UPPGameInstance::CreateGame()
{
	if (!SessionInterface.IsValid()) return;

	auto ExistingSession = SessionInterface->GetNamedSession(SESSION_NAME);
	if (ExistingSession != nullptr)
	{
		bCreateNewSession = 1;
		SessionInterface->DestroySession(SESSION_NAME);
	}
	else
	{
		CreateSession();
	}
}

void UPPGameInstance::DestroyGame()
{
	if (!SessionInterface.IsValid()) return;

	auto ExistingSession = SessionInterface->GetNamedSession(SESSION_NAME);
	if (ExistingSession != nullptr)
	{
		bCreateNewSession = 0;
		SessionInterface->DestroySession(SESSION_NAME);
	}
}

void UPPGameInstance::JoinGame(int32 Index)
{
	if (!SessionInterface.IsValid()) return;

	if (!SessionSearch.IsValid()) return;

	SessionInterface->JoinSession(0, SESSION_NAME, SessionSearch->SearchResults[Index]);
}

void UPPGameInstance::StartGame()
{
	if (!SessionInterface.IsValid()) return;

	SessionInterface->StartSession(SESSION_NAME);

	ServerGoTo(GameLevel);
}

void UPPGameInstance::CreateSession()
{
	if (!SessionInterface.IsValid()) return;

	bCreateNewSession = 0;

	FOnlineSessionSettings SessionSettings;
	SessionSettings.bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";
	SessionSettings.NumPublicConnections = 5;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bUsesPresence = true;
	////// TODO SessionSettings.Set(SERVER_NAME_SETTINGS_KEY, DesiredServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	SessionInterface->CreateSession(0, SESSION_NAME, SessionSettings);
}

bool UPPGameInstance::IsLevel(const FSoftObjectPath& softPath) const
{
	if (UWorld* world = GetWorld())
	{
		FString worldPath = world->RemovePIEPrefix(world->GetPathName());
		return softPath == worldPath;
	}

	return false;
}

FString UPPGameInstance::GetAssetPathFromSoftPath(const FSoftObjectPath& softPath) const
{
	FString result = softPath.GetAssetPathString();
	result.RemoveFromEnd("." + softPath.GetAssetName());
	return result;
}

void UPPGameInstance::OnWorldBeginPlay()
{
	if (IsMenu())
	{
		if (bCreateNewSession) return CreateSession();

		SetCurrentWidget(MenuWidgetClass);
	}
	else if (IsLobby())
	{
		if (GetWorld()->IsServer())
		{
			SetCurrentWidget(LobbyWidgetClass);
		}
		else
		{
			////// TODO Add progress widget (CLIENT) when we are almost starting
		}
	}
}

void UPPGameInstance::SetCurrentWidget(const TSubclassOf<UUserWidget>& userWidgetClass)
{
	if (!CurrentWidget && !userWidgetClass) return;

	if (CurrentWidget && userWidgetClass && CurrentWidget->GetClass() == userWidgetClass) return;

	if (CurrentWidget)
	{
		if (APlayerController* PlayerController = GetFirstLocalPlayerController())
		{
			FInputModeGameOnly InputModeData;
			PlayerController->SetInputMode(InputModeData);

			PlayerController->bShowMouseCursor = false;
		}

		CurrentWidget->RemoveFromViewport();
		CurrentWidget = nullptr;
	}

	if (UUserWidget* userWidget = userWidgetClass ? CreateWidget<UUserWidget>(this, userWidgetClass) : nullptr)
	{
		CurrentWidget = userWidget;
		CurrentWidget->AddToViewport();

		if (APlayerController* PlayerController = GetFirstLocalPlayerController())
		{
			FInputModeUIOnly InputModeData;
			InputModeData.SetWidgetToFocus(CurrentWidget->TakeWidget());
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

			PlayerController->SetInputMode(InputModeData);

			PlayerController->bShowMouseCursor = true;
		}
	}
}

void UPPGameInstance::OnCreateSessionComplete(FName SessionName, bool Success)
{
	if (Success) return ServerGoTo(LobbyLevel);
	UE_LOG(LogTemp, Warning, TEXT("Could not create session"));
}

void UPPGameInstance::OnDestroySessionComplete(FName SessionName, bool Success)
{
	if (Success) return ClientGoTo(MenuLevel);
	UE_LOG(LogTemp, Warning, TEXT("Could not destroy session"));
}

void UPPGameInstance::OnFindSessionsComplete(bool Success)
{
	bIsRefreshing = 0;

	if (Success && SessionSearch.IsValid() && IsMenu())
	{
		UE_LOG(LogTemp, Warning, TEXT("Finished Find Session"));

		if (UMenuWidget* menuWidget = Cast<UMenuWidget>(CurrentWidget))
		{
			static uint8 counter = 0;

			for (size_t i = 0; i < SessionSearch->SearchResults.Num(); i++)
			{
				FString owningUserName = SessionSearch->SearchResults[i].Session.OwningUserName;
				menuWidget->AddGameToJoin(i, owningUserName, counter);
			}

			counter++;
		}
	}
}

void UPPGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (!SessionInterface.IsValid()) return;

	FString Address;
	if (SessionInterface->GetResolvedConnectString(SessionName, Address)) return ClientGoTo(Address);
	
	UE_LOG(LogTemp, Warning, TEXT("Could not get connect string."));
}

void UPPGameInstance::ClientGoTo(FString address)
{
	SetCurrentWidget(nullptr);

	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	PlayerController->ClientTravel(address, ETravelType::TRAVEL_Absolute);
}

void UPPGameInstance::ServerGoTo(const FSoftObjectPath& softPath)
{
	SetCurrentWidget(nullptr);

	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;

	World->ServerTravel(GetAssetPathFromSoftPath(softPath) + "?listen");
}

void UPPGameInstance::OnRefresh()
{
	if (!SessionInterface.IsValid()) return;

	SessionSearch = MakeShareable(new FOnlineSessionSearch);

	if (SessionSearch && !bIsRefreshing)
	{
		bIsRefreshing = 1;
		
		//SessionSearch->bIsLanQuery = true;
		SessionSearch->MaxSearchResults = 100;
		SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
		UE_LOG(LogTemp, Warning, TEXT("Starting Find Session"));
		
		SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
	}
}