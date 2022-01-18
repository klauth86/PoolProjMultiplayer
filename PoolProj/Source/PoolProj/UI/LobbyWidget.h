// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UI/BaseWidget.h"
#include "LobbyWidget.generated.h"

UCLASS()
class POOLPROJ_API ULobbyWidget : public UBaseWidget
{
	GENERATED_BODY()

protected:

	UFUNCTION(BlueprintCallable, Category = "MenuWidget")
		void Back();
};