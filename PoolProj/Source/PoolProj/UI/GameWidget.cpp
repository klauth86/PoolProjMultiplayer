// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/GameWidget.h"
#include "ActionRouter.h"

void UGameWidget::NativeConstruct()
{
	Super::NativeConstruct();


}

void UGameWidget::NativeDestruct()
{


	Super::NativeDestruct();
}

void UGameWidget::OnPrepared() { ActionRouter::Client_OnPrepared.Broadcast(GetOwningPlayer()); }