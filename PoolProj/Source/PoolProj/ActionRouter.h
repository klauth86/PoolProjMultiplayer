// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/ObjectMacros.h"

DECLARE_DELEGATE(FToggleEvent);
DECLARE_DELEGATE_OneParam(FJoinGameEvent, int32);

namespace ActionRouter
{
	extern FToggleEvent OnCreateGame;

	extern FToggleEvent OnDestroyGame;

	extern FJoinGameEvent OnJoinGame;

	extern FToggleEvent OnStartGame;

	extern FToggleEvent OnRefresh;
};