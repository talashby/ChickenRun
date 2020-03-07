// Fill out your copyright notice in the Description page of Project Settings.

#include "ChickenRun.h"
#include "ACMGeneral.h"
#include "ChickenRunCharacter.h"
#include "ACMCharacter.h"

static const FName g_ssGameMenu("GameMenu");

ACMGeneral::ACMGeneral(class AChickenRunGameMode *owner) : ACMachine("ACMGeneral")
{
	pOwnerActor = owner;
    REGISTER_ACSTATE(ACMGeneral, GameMenu);
}

ACMGeneral::~ACMGeneral()
{
}

FName ACMGeneral::TickStateStart(int iTickType)
{
    if (ACMachine::TICK_StateStarted == iTickType)
	{		
        return g_ssGameMenu; // ******************************* State Finished ********************************
    }
    return "";
}

FName ACMGeneral::TickStateGameMenu(int iTickType)
{
	if (ACMachine::TICK_StateStarted == iTickType)
	{
		spACMCharacter = SP_ACMCharacter(new ACMCharacter(pOwnerActor));
	}
	else if (ACMachine::TICK_StateNormal == iTickType)
	{
		spACMCharacter->TickPublic();
	}
	return "";
}
