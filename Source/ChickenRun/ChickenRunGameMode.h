// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameMode.h"
#include "ChickenRunGameMode.generated.h"

class ACMGeneral; typedef TSharedPtr<ACMGeneral> SP_ACMGeneral;

UCLASS(minimalapi)
class AChickenRunGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AChickenRunGameMode();
	class AGameField *GetGameField() { return pGameField; }

	virtual void InitGameState() override;
	virtual void StartPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	SP_ACMGeneral GetACMGeneral() const { return spACMGeneral; }
	UFUNCTION(exec)
	virtual void ChRHideTiles();
	UFUNCTION(exec)
	virtual void ChRStopAnims();
	UFUNCTION(exec)
	virtual void ChRGetCurrentTileName();

private:
	class AGameField *pGameField;
	SP_ACMGeneral spACMGeneral;
};



