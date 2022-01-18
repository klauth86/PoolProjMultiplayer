// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/MenuWidget.h"
#include "UI/JoinWidget.h"
#include "ActionRouter.h"

void UMenuWidget::AddGameToJoin(int32 index, const FString& owningUserName, uint8 counter) {
	if (GamesToJoin.Contains(owningUserName))
	{
		GamesToJoin[owningUserName]->Init(index, owningUserName, counter);
	}
	else
	{
		if (UJoinWidget* joinWidget = CreateWidget<UJoinWidget>(this, JoinWidgetClass))
		{
			joinWidget->Init(index, owningUserName, counter);
			joinWidget->PostInit();

			ApplyFilter(joinWidget);

			GamesToJoin.Add(owningUserName, joinWidget);
			AddGameToJoin(joinWidget);
		}
	}

	for (TMap<FString, UJoinWidget*>::TIterator It(GamesToJoin); It; ++It)
	{
		if (It.Value()->IsOutdated(counter))
		{
			It.Value()->RemoveFromParent();
			It.RemoveCurrent();
		}
	}
}

void UMenuWidget::CreateGame() { ActionRouter::OnCreateGame.ExecuteIfBound(); }

void UMenuWidget::ApplyFilter(UJoinWidget* joinWidget)
{
	if (joinWidget) return joinWidget->ApplyFilter(FilterText);

	for (TMap<FString, UJoinWidget*>::TIterator It(GamesToJoin); It; ++It) It.Value()->ApplyFilter(FilterText);
}

void UMenuWidget::Refresh() { ActionRouter::OnRefresh.ExecuteIfBound(); }