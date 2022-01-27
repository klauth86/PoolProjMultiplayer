// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/ObjectMacros.h"

DECLARE_DELEGATE(FToggleEvent);
DECLARE_DELEGATE_OneParam(FJoinGameEvent, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FShotEvent, UClass* ballClass);

namespace ActionRouter
{
	extern FToggleEvent OnCreateGame;

	extern FToggleEvent OnDestroyGame;

	extern FJoinGameEvent OnJoinGame;

	extern FToggleEvent OnStartGame;

	extern FToggleEvent OnRefresh;

	extern FToggleEvent Server_OnPlayerPrepared;

	extern FToggleEvent Server_OnStartNextTurn;

	extern FToggleEvent Server_OnStartNextTurn;

	extern FShotEvent Server_OnShot;
};