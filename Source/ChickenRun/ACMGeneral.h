// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "StateMachine/ACMachine.h"
/**
 * 
 */
class ACMCharacter; typedef TSharedPtr<ACMCharacter> SP_ACMCharacter;

class ACMGeneral : public ACMachine
{
public:
	ACMGeneral(class AChickenRunGameMode *owner);
	virtual ~ACMGeneral();

	SP_ACMCharacter GetACMCharacter() const{ return spACMCharacter; }

protected:
    virtual FName TickStateStart(int iTickType);
	FName TickStateGameMenu(int iTickType);

private:
	SP_ACMCharacter spACMCharacter;
	class AChickenRunGameMode *pOwnerActor;
	//class ABreak2BricksBlockGrid *pGridActor;
};

class ACMGeneral; typedef TSharedPtr<ACMGeneral> SP_ACMGeneral;
