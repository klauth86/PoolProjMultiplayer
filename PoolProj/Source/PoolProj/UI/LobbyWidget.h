// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "LobbyWidget.generated.h"

UCLASS()
class POOLPROJ_API ULobbyWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	UFUNCTION(BlueprintCallable, Category = "MenuWidget")
		void Back();
};