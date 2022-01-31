// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "GameWidget.generated.h"

UCLASS()
class POOLPROJ_API UGameWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	void NativeConstruct() override;

	void NativeDestruct() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "GameWidget")
		void OnStartTurn();

	UFUNCTION(BlueprintImplementableEvent, Category = "GameWidget")
		void OnOpponentTurn();

	UFUNCTION(BlueprintImplementableEvent, Category = "GameWidget")
		void OnStrength(float strength);

protected:

	UFUNCTION(BlueprintCallable, Category = "GameWidget")
		void OnPrepared();
};