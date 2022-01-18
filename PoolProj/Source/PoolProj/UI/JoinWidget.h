// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "JoinWidget.generated.h"

UCLASS()
class POOLPROJ_API UJoinWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintImplementableEvent, Category = "JoinWidget")
		void PostInit();

	void Init(int32 index, const FString& owningUserName, uint8 counter) { Index = index; OwningUserName = owningUserName; Counter = counter; }

	bool IsOutdated(uint8 counter) const { return Counter != counter; }

	void ApplyFilter(const FText& filter) { const FString filterString = filter.ToString(); SetVisibility(filterString.IsEmpty() || OwningUserName.Contains(filterString) ? ESlateVisibility::Visible : ESlateVisibility::Collapsed); }

protected:

	UFUNCTION(BlueprintCallable, Category = "JoinWidget")
		void Join();

protected:

	int32 Index;

	UPROPERTY(BlueprintReadOnly, Category = "JoinWidget")
		FString OwningUserName;

	uint8 Counter;
};
