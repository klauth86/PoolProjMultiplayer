// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UI/BaseWidget.h"
#include "MenuWidget.generated.h"

class UJoinWidget;

UCLASS()
class POOLPROJ_API UMenuWidget : public UBaseWidget
{
	GENERATED_BODY()

public:

	void AddGameToJoin(int32 index, const FString& owningUserName, uint8 counter);

protected:

	UFUNCTION(BlueprintImplementableEvent, Category = "JoinWidget")
		void AddGameToJoin(UUserWidget* widget);

	UFUNCTION(BlueprintCallable, Category = "MenuWidget")
		void CreateGame();

	UFUNCTION(BlueprintCallable, Category = "MenuWidget")
		void Filter(const FText& filterText) { FilterText = filterText; ApplyFilter(nullptr); }

	void ApplyFilter(UJoinWidget* joinWidget);

	UFUNCTION(BlueprintCallable, Category = "MenuWidget")
		void Refresh();

protected:

	UPROPERTY(EditDefaultsOnly, Category = "MenuWidget")
		TSubclassOf<UJoinWidget> JoinWidgetClass;

	UPROPERTY()
		TMap<FString, UJoinWidget*> GamesToJoin;

	FText FilterText;
};