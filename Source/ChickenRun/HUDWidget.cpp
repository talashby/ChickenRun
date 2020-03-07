// Fill out your copyright notice in the Description page of Project Settings.

#include "ChickenRun.h"
#include "HUDWidget.h"


UHUDWidget::UHUDWidget() : UUserWidget(FObjectInitializer())
{
	pHUDWidget = nullptr;
	iCoinsNum = 0;
}

void UHUDWidget::SetStepNum(int32 iStepNum)
{
	StepNum = FText::FromString(FString::FromInt(iStepNum));
}

void UHUDWidget::IncCoins()
{
	++iCoinsNum;
	CoinsNum = FText::FromString(FString::FromInt(iCoinsNum));
}

void UHUDWidget::ClearCoins()
{
	iCoinsNum = 0;
	CoinsNum = FText::FromString(TEXT("0"));
}

