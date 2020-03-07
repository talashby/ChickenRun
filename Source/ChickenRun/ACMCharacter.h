// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "StateMachine/ACMachine.h"
/**
 * 
 */
class AChickenRunCharacter;
class AClickActor;

typedef std::vector<AClickActor*> tBlockVector;

class ACMCharacter : public ACMachine
{
public:
	ACMCharacter(class AChickenRunGameMode *owner);
	virtual ~ACMCharacter();

	void JumpBackward();
	void JumpForward();
	void JumpLeft();
	void JumpRight();
	void Move(FIntVector vecMoveDirection);
	//void ClickedCell(int32 iX, int32 iY);
	void ReleasedCell(int32 iX, int32 iY);
	FString GetCurrentTileName() const; // for console

protected:
	void Tick() override;
    virtual FName TickStateStart(int iTickType) override;
	FName TickStateGame(int iTickType);
	FName TickStateMoving(int iTickType);
	FName TickStateRotation(int iTickType);
	FName TickStateRotateBeforeJump(int iTickType);
	FName TickStateJump(int iTickType);
	FName TickStateJump2(int iTickType);
	FName TickStateWaitFalling(int iTickType);
	FName TickStateWaterDrown(int iTickType);
	FName TickStateLogSail(int iTickType);
	FName TickStateLogMoving(int iTickType);
	virtual void BeforeTickStateStarted() override;
private:
	float AdjustValue(float fFrom, float fTo, float fSpeedInPixelsPerSec=50);
	void AdjustPosition(FVector vecBlockLoc, float fSpeedInPixelsPerSec = 50);

	FName NextNavigationState();
	bool CanMove(FIntVector vecWhere);
	bool CanJump(FIntVector vecWhere);
	void ChangePosition(int32 iNewXPos, int32 iNewYPos);
	bool IsOnLog();
	bool LogInteraction();
	bool IsCarCollision(class AMyStaticMeshActor **pCoinActor) const;

	enum LOCAL_EXTERNAL_SIGNALS
	{
		eSignal_Clicked = EXTERNAL_SIGNALS_END,
		eSignal_Clicked_Screen,
		eSignal_JumpBackward,
		eSignal_JumpForward,
		eSignal_JumpLeft,
		eSignal_JumpRight
	};
	class AChickenRunGameMode *pOwnerActor;
	class AChickenRunCharacter *pPawn;
	//class AGameField *pGameField;
	class AGeneralSettings *pAGeneralSettings;

	FIntVector vecOrientation;
	FIntVector vecDesiredOrientation; // reset before any state start
	int32 iXPos;
	int32 iYPos;

	AActor *pCurrentLogActor; // current interaction log
	float pCurrentLogYLocation;

	AActor *pStateLogSail_LogActor;
	float fStateLogSail_DesiredY; // relative

	//int iClicked;
	FVector2D vecClickedPos;
	FIntVector vecClickedScreenMoveDirection;
	//float fAutoclickTimer;
	float fWaterDrawnTime;

	float fBurnTime;
	int32 iBurnTimeInPercent;
};

class ACMCharacter; typedef TSharedPtr<ACMCharacter> SP_ACMCharacter;