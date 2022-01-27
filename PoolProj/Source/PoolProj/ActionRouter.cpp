// Fill out your copyright notice in the Description page of Project Settings.

#include "ActionRouter.h"

FToggleEvent ActionRouter::OnCreateGame;

FToggleEvent ActionRouter::OnDestroyGame;

FJoinGameEvent ActionRouter::OnJoinGame;

FToggleEvent ActionRouter::OnStartGame;

FToggleEvent ActionRouter::OnRefresh;

FToggleEvent ActionRouter::Server_OnPlayerPrepared;

FToggleEvent ActionRouter::Server_OnStartNextTurn;

FShotEvent ActionRouter::Server_OnShot;

FPawnEvent ActionRouter::Client_OnPrepared;