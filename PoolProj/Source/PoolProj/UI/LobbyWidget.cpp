// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/LobbyWidget.h"
#include "ActionRouter.h"

void ULobbyWidget::Back() { ActionRouter::OnDestroyGame.ExecuteIfBound(); }