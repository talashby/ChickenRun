// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "HUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class CHICKENRUN_API UHUDWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UHUDWidget();
	UPROPERTY()  // used for prevent garbage collection
	class UHUDWidget *pHUDWidget;

	UPROPERTY(Category = LevelMenu, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FText StepNum;
	void SetStepNum(int32 iStepNum);

	UPROPERTY(Category = LevelMenu, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FText CoinsNum;
	void IncCoins();
	void ClearCoins();

private:
	int32 iCoinsNum;
};
