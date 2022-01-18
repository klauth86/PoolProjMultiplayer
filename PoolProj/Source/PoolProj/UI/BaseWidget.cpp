// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/BaseWidget.h"
#include "ActionRouter.h"

void UBaseWidget::NativeConstruct()
{
	Super::NativeConstruct();
	ActionRouter::OnUIConstruct.ExecuteIfBound();
}

void UBaseWidget::NativeDestruct()
{
	ActionRouter::OnUIDestruct.ExecuteIfBound();
	Super::NativeDestruct();
}