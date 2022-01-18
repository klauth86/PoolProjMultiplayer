// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/JoinWidget.h"
#include "ActionRouter.h"

void UJoinWidget::Join() { ActionRouter::OnJoinGame.ExecuteIfBound(Index); }