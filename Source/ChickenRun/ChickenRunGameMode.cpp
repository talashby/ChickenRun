// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "ChickenRun.h"
#include "ChickenRunGameMode.h"
#include "ChickenRunPlayerController.h"
#include "GameField.h"
#include "GeneralSettings.h"
#include "ClickActor.h"
#include "ACMGeneral.h"
#include "ACMCharacter.h"

AChickenRunGameMode::AChickenRunGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = AChickenRunPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDownCPP/Blueprints/TopDownCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	pGameField = nullptr;
}

void AChickenRunGameMode::InitGameState()
{
	AGeneralSettings *pAGeneralSettings;
	if (MyHelper::GetActorFromScene(GetWorld(), "GeneralSettings", pAGeneralSettings))
	{
		if (pAGeneralSettings->GameFieldSingle)
		{
			AClickActor::SetGameField(pAGeneralSettings->GameFieldSingle);
		}
		else if (pAGeneralSettings->GameFieldSequence.Num() && pAGeneralSettings->GameFieldSequence[0].GameField)
		{
			AClickActor::SetGameField(pAGeneralSettings->GameFieldSequence[0].GameField);
		}
	}
}

void AChickenRunGameMode::StartPlay()
{
	Super::StartPlay();
	spACMGeneral = SP_ACMGeneral(new ACMGeneral(this));
	spACMGeneral->TickPublic();
}

void AChickenRunGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	spACMGeneral->TickPublic();
}

void AChickenRunGameMode::ChRHideTiles()
{
	pGameField->ConsoleCommandHideTiles();
}

void AChickenRunGameMode::ChRStopAnims()
{
	pGameField->ConsoleCommandStopAnims();
}

void AChickenRunGameMode::ChRGetCurrentTileName()
{
/*
	FConstPawnIterator it = GetWorld()->GetPawnIterator();
	AChickenRunCharacter *pPawn = Cast<AChickenRunCharacter>(it->Get());
	SP_ACMGeneral spACMGeneral = pPawn->GetACMGeneral();*/
	if (nullptr != spACMGeneral.Get())
	{
		SP_ACMCharacter spACMCharacter = spACMGeneral->GetACMCharacter();
		if (nullptr != spACMCharacter.Get())
		{
			FString sTileName = spACMCharacter->GetCurrentTileName();
			APlayerController *pPlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
			pPlayerController->ClientMessage(sTileName);
		}
	}
}
