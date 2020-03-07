// Fill out your copyright notice in the Description page of Project Settings.

#include "ChickenRun.h"
#include "ACMCharacter.h"

#include "ChickenRunGameMode.h"
#include "ChickenRunCharacter.h"
#include "ClickActor.h"
#include "GameField.h"
#include "GeneralSettings.h"
#include "MyStaticMeshActor.h"
#include "HUDWidget.h"

static const FName g_ssGame("Game");
static const FName g_ssMoving("Moving");
static const FName g_ssRotation("Rotation");
static const FName g_ssRotateBeforeJump("RotateBeforeJump");
static const FName g_ssJump("Jump");
static const FName g_ssJump2("Jump2");
static const FName g_ssWaitFalling("WaitFalling");
static const FName g_ssWaterDrown("WaterDrown");
static const FName g_ssLogSail("LogSail");
static const FName g_ssLogMoving("LogMoving");

ACMCharacter::ACMCharacter(AChickenRunGameMode *owner) : ACMachine("ACMCharacter")
{
	pOwnerActor = owner;
    REGISTER_ACSTATE(ACMCharacter, Game);
	REGISTER_ACSTATE(ACMCharacter, Moving);
	REGISTER_ACSTATE(ACMCharacter, Rotation);
	REGISTER_ACSTATE(ACMCharacter, RotateBeforeJump);
	REGISTER_ACSTATE(ACMCharacter, Jump);
	REGISTER_ACSTATE(ACMCharacter, Jump2);
	REGISTER_ACSTATE(ACMCharacter, WaitFalling);
	REGISTER_ACSTATE(ACMCharacter, WaterDrown);
	REGISTER_ACSTATE(ACMCharacter, LogSail);
	REGISTER_ACSTATE(ACMCharacter, LogMoving);

	iXPos = -1;
	iYPos = -1;

	pCurrentLogActor = nullptr;
	pCurrentLogYLocation = 0;

	TActorIterator<AChickenRunCharacter> it(pOwnerActor->GetWorld());
	pPawn = *it;
//	iClicked = 0;
}

ACMCharacter::~ACMCharacter()
{
}

float ACMCharacter::AdjustValue(float fFrom, float fTo, float fSpeedInPixelsPerSec)
{
	float fResult = fFrom;
	if (fResult < fTo)
	{
		fResult += pOwnerActor->GetWorld()->GetDeltaSeconds() * fSpeedInPixelsPerSec;
		if (fResult > fTo)
		{
			fResult = fTo;
		}
	}
	else if (fResult > fTo)
	{
		fResult -= pOwnerActor->GetWorld()->GetDeltaSeconds() * fSpeedInPixelsPerSec;
		if (fResult < fTo)
		{
			fResult = fTo;
		}
	}
	return fResult;
}

void ACMCharacter::AdjustPosition(FVector vecBlockLoc, float fSpeedInPixelsPerSec /*= 50*/)
{
	FVector vecLoc = pPawn->GetActorLocation();
	if (0 != vecOrientation.X)
	{
		vecLoc.Y = AdjustValue(vecLoc.Y, vecBlockLoc.Y, fSpeedInPixelsPerSec);
	}
	if (0 != vecOrientation.Y)
	{
		vecLoc.X = AdjustValue(vecLoc.X, vecBlockLoc.X, fSpeedInPixelsPerSec);
	}
	pPawn->SetActorLocation(vecLoc);
}

/*void ACMCharacter::ClickedCell(int32 iX, int32 iY)
{
	++iClicked;
	vecClickedPos = FVector2D(iX, iY);
	//fAutoclickTimer = 0;
	SetStateExternalSignal(eSignal_Clicked);
	//if (IsCurrentState(g_ssGame))

}*/

void ACMCharacter::ReleasedCell(int32 iX, int32 iY)
{
	/*--iClicked;
	if (iClicked < 0)
	{
	iClicked = 0;
	}*/
	if (IsStateExternalSignal(EXTERNAL_SIGNAL_None))
	{
		vecClickedPos = FVector2D(iX, iY);
		SetStateExternalSignal(eSignal_Clicked);
	}
}

FString ACMCharacter::GetCurrentTileName() const
{
	return pAGeneralSettings->GetClickActor(iXPos, iYPos)->GetName();
}

void ACMCharacter::JumpBackward()
{
	SetStateExternalSignal(eSignal_JumpBackward);
}

void ACMCharacter::JumpForward()
{
	SetStateExternalSignal(eSignal_JumpForward);
}

void ACMCharacter::JumpLeft()
{
	SetStateExternalSignal(eSignal_JumpLeft);
}

void ACMCharacter::JumpRight()
{
	SetStateExternalSignal(eSignal_JumpRight);
}

void ACMCharacter::Move(FIntVector vecMoveDirection)
{
	vecClickedScreenMoveDirection = vecMoveDirection;
	SetStateExternalSignal(eSignal_Clicked_Screen);
}

FName ACMCharacter::NextNavigationState()
{
	if (IsStateExternalSignal(EXTERNAL_SIGNAL_None))
	{
		return "";
	}
	if (IsStateExternalSignal(eSignal_JumpBackward))
	{
		if (vecOrientation.X != -1)
		{
			vecDesiredOrientation = FIntVector(-1, 0, 0);
			return g_ssRotateBeforeJump;
		}
		else
		{
			return g_ssJump;
		}
	}
	else if (IsStateExternalSignal(eSignal_JumpForward))
	{
		if (vecOrientation.X != 1)
		{
			vecDesiredOrientation = FIntVector(1, 0, 0);
			return g_ssRotateBeforeJump;
		}
		else
		{
			return g_ssJump;
		}
	}
	else if (!pAGeneralSettings->IsSimpleControl() && IsStateExternalSignal(eSignal_JumpLeft))
	{
		if (vecOrientation.Y != -1)
		{
			vecDesiredOrientation = FIntVector(0, -1, 0);
			return g_ssRotateBeforeJump;
		}
		else
		{
			return g_ssJump;
		}
	}
	else if (!pAGeneralSettings->IsSimpleControl() && IsStateExternalSignal(eSignal_JumpRight))
	{
		if (vecOrientation.Y != 1)
		{
			vecDesiredOrientation = FIntVector(0, 1, 0);
			return g_ssRotateBeforeJump;
		}
		else
		{
			return g_ssJump;
		}
	}
	else if (IsStateExternalSignal(eSignal_Clicked_Screen))
	{
		vecDesiredOrientation = vecClickedScreenMoveDirection;
		return g_ssMoving;
	}
	if (IsStateExternalSignal(eSignal_Clicked))
	{

		int iX = vecClickedPos.X;
		int iY = vecClickedPos.Y;
		if (pAGeneralSettings->IsSimpleControl())
		{
			if (iXPos < iX)
			{ // forward
				vecDesiredOrientation = FIntVector(1, 0, 0);
				return g_ssMoving; // ******************************* State Finished ********************************
			}
			else if (iXPos > iX)
			{ // backward
				vecDesiredOrientation = FIntVector(-1, 0, 0);
				return g_ssMoving; // ******************************* State Finished ********************************
			}
		}
		else
		{
			/*if (iXPos == iX && iYPos == iY)
			{ // pawn clicked 
				vecDesiredOrientation = vecOrientation;
				return g_ssRotateBeforeJump; // ******************************* State Finished ********************************
			}
			else*/ if (iXPos < iX && abs(iY - iYPos) < iX - iXPos)
			{
				if (abs(iY - iYPos) == iX - iXPos)
				{ // do nothing
				}
				else
				{ // forward
					vecDesiredOrientation = FIntVector(1, 0, 0);
					return g_ssMoving; // ******************************* State Finished ********************************
				}
			}
			else if (iXPos > iX && abs(iYPos - iY) < iXPos - iX)
			{
				if (abs(iYPos - iY) == iXPos - iX)
				{  // do nothing
				}
				else
				{ // backward
					vecDesiredOrientation = FIntVector(-1, 0, 0);
					return g_ssMoving; // ******************************* State Finished ********************************
				}
			}
			else if (iYPos < iY)
			{ // right
				vecDesiredOrientation = FIntVector(0, 1, 0);
				return g_ssMoving; // ******************************* State Finished ********************************
			}
			else if (iYPos > iY)
			{ // left
				vecDesiredOrientation = FIntVector(0, -1, 0);
				return g_ssMoving; // ******************************* State Finished ********************************
			}

			//OperateStates();
		}
	}
	SetStateExternalSignal(EXTERNAL_SIGNAL_None);
	return "";		
}

bool ACMCharacter::CanMove(FIntVector vecWhere)
{
	int32 iNewXPos = iXPos + vecWhere.X;
	int32 iNewYPos = iYPos + vecWhere.Y;
	AClickActor *pAClickActor = pAGeneralSettings->GetClickActor(iNewXPos, iNewYPos);
	if (pAClickActor == nullptr)
	{
		return false;
	}
	if (AClickActor::eGroundType_Block == pAClickActor->GetGroundType() || AClickActor::eGroundType_SmallBlock == pAClickActor->GetGroundType())
	{
		return false;
	}
	return true;
}

bool ACMCharacter::CanJump(FIntVector vecWhere)
{
	int32 iNewXPos = iXPos + vecWhere.X;
	int32 iNewYPos = iYPos + vecWhere.Y;
	AClickActor *pAClickActor = pAGeneralSettings->GetClickActor(iNewXPos, iNewYPos);
	if (pAClickActor == nullptr)
	{
		return false;
	}
	if (AClickActor::eGroundType_Block == pAClickActor->GetGroundType())
	{
		return false;
	}
	return true;
}

void ACMCharacter::ChangePosition(int32 iNewXPos, int32 iNewYPos)
{
	iXPos = iNewXPos;
	iYPos = iNewYPos;
	pAGeneralSettings->PawnPositionChanged(iXPos, iYPos);
	//pGameField->PawnPositionChanged(iXPos, iYPos);
}

bool ACMCharacter::IsOnLog()
{
//	pPawn->UpdateOverlaps();
	UPrimitiveComponent *pComponent = pPawn->GetMovementBase();
	if (pComponent != nullptr)
	{
		UClass *pMovementBaseClass = pComponent->GetOwner()->GetClass();
		if (pMovementBaseClass->IsChildOf(pAGeneralSettings->GetLogClass()))
		{
			pCurrentLogActor = pComponent->GetOwner();
			//GEngine->AddOnScreenDebugMessage(0, 5.f, FColor::Yellow, TEXT("On log!"));
			return true;
		}
	}
	//GEngine->AddOnScreenDebugMessage(0, 5.f, FColor::Yellow, TEXT("Not on log!"));
	return false;
}

bool ACMCharacter::LogInteraction()
{
	UPrimitiveComponent *pComponent = pPawn->GetMovementBase();
	if (pComponent != nullptr)
	{
		UClass *pMovementBaseClass = pComponent->GetOwner()->GetClass();
		if (pMovementBaseClass->IsChildOf(pAGeneralSettings->GetLogClass()))
		{
			if (pCurrentLogActor != pComponent->GetOwner())
			{
				pCurrentLogActor = pComponent->GetOwner();
				FVector vecOrigin, vecBoxExtent;
				pCurrentLogActor->GetActorBounds(false, vecOrigin, vecBoxExtent);
				float fDiff = (pPawn->GetActorLocation().Y - pCurrentLogActor->GetActorLocation().Y) / pAGeneralSettings->GetTileSize();
				if (0 == (int)vecBoxExtent.Y % 100) // vecBoxExtent.Y is 50 for 1x log, 100 for 2x log, 150 for 3x log
				{
					fDiff = roundf(fDiff + 0.5f) - 0.5f;
				}
				else
				{
					fDiff = roundf(fDiff);
				}
				pCurrentLogYLocation = fDiff*pAGeneralSettings->GetTileSize();
			}
			FVector vecLoc = pPawn->GetActorLocation();
			FVector vecLogLoc = pCurrentLogActor->GetActorLocation();
			vecLoc.Y = AdjustValue(vecLoc.Y, vecLogLoc.Y + pCurrentLogYLocation, 200);
			pPawn->SetActorLocation(vecLoc);
			return true;
		}
		else
		{
			pCurrentLogActor = nullptr;
		}
	}
	return false;
}

bool ACMCharacter::IsCarCollision(AMyStaticMeshActor **pCoinActor) const
{
	*pCoinActor = nullptr;
	// init start visible map
	TSet<AActor*> aOverlapActors;
	pPawn->UpdateOverlaps();
	pPawn->GetOverlappingActors(aOverlapActors);

	for (TSet<AActor*>::TConstIterator Itr(aOverlapActors); Itr; ++Itr)
	{
		AActor *pActor = *Itr;
		UClass *pClass = pActor->GetClass();
 		if (pClass->IsChildOf(pAGeneralSettings->GetCarClass()))
		{
			return true;
		}
		else if (pClass->IsChildOf(pAGeneralSettings->GetCoinClass()))
		{
			*pCoinActor = Cast<AMyStaticMeshActor>(pActor);
		}
	}
	return false;
}

void ACMCharacter::Tick()
{
	if (IsInErrorState())
	{
		return;
	}
	FVector vecCellLoc = pAGeneralSettings->GetClickActor(iXPos, iYPos)->GetActorLocation();
	FVector vecPawnLoc = pPawn->GetActorLocation();
	if ((vecPawnLoc - vecCellLoc).Size() > 500)
	{
		GEngine->AddOnScreenDebugMessage(0, 5.f, FColor::Yellow, TEXT("BAD PAWN POSITION!"));
	}
	if (!IsCurrentState(GetStateStartName()))
	{
		check(pAGeneralSettings != nullptr);
		if (pAGeneralSettings != nullptr)
		{
			FVector vecLoc = pPawn->GetActorLocation();
			if (vecLoc.Z < pAGeneralSettings->GetGameFieldZ() - 200)
			{
				SetRestart(); // ******************************* State Finished ********************************
			}
		}
	}

	AMyStaticMeshActor *pCoin = nullptr;
	if (IsCarCollision(&pCoin))
	{
		WriteLogMessage(ELogVerbosity::Display, "Car collision!");
		SetRestart(); // ******************************* State Finished ********************************
		return;
	}
	if (pCoin)
	{
		pAGeneralSettings->GetHUDWidget()->IncCoins();
		if (pCoin->pOwnerTile)
		{
			pCoin->pOwnerTile->DestroyCoin(pCoin);
		}
		else
		{
			pCoin->SetActorHiddenInGame(true);
			pCoin->SetActorEnableCollision(false);
		}
	}

	if (AClickActor::eGrassType_Fire == pAGeneralSettings->GetClickActor(iXPos, iYPos)->GetGroundTypeSecond())
	{
		fBurnTime += pOwnerActor->GetWorld()->GetDeltaSeconds();
		if (fBurnTime >= pAGeneralSettings->BurnTime)
		{
			SetRestart();
			return;
		}
	}
	else
	{
		fBurnTime -= pOwnerActor->GetWorld()->GetDeltaSeconds();
		if (fBurnTime < 0)
		{
			fBurnTime = 0;
		}
	}
	int32 iInPercent = 100 * FPlatformMath::RoundToInt(fBurnTime / pAGeneralSettings->BurnTime);
	if (iInPercent > 100)
	{
		iInPercent = 100;
	}
	if (iBurnTimeInPercent != iInPercent)
	{
		iBurnTimeInPercent = iInPercent;
		pPawn->AnimBurn(iBurnTimeInPercent);
	}
}

void ACMCharacter::BeforeTickStateStarted()
{
}

FName ACMCharacter::TickStateStart(int iTickType)
{	
    if (ACMachine::TICK_StateStarted == iTickType)
	{
		fBurnTime = 0;
//		iClicked = 0;
		/*{
			AGameModeBase *pGameMode = pOwnerActor->GetWorld()->GetAuthGameMode();
			AChickenRunGameMode *pCRGameMode = Cast<AChickenRunGameMode>(pGameMode);
			pGameField = pCRGameMode->GetGameField();
			if (!pGameField)
			{
				return ErrorState("AGameField not found");
			}
		}*/
		{
			if (!MyHelper::GetActorFromScene(pOwnerActor->GetWorld(), "GeneralSettings", pAGeneralSettings))
			{
				return ErrorState("GeneralSettings not found");
			}
		}
/*
		{
			TActorIterator<ATriggerVolume> ActorItr = TActorIterator<ATriggerVolume>(pOwnerActor->GetWorld());
			if (ActorItr)
			{
				ATriggerVolume *pTriggerVolume = *ActorItr;
				TSet<AActor*> aOverlapActors;
				pTriggerVolume->UpdateOverlaps();
				pTriggerVolume->GetOverlappingActors(aOverlapActors);
				int ttt = 0;
			}
			else
			{
				return ErrorState("AGameField not found");
			}
		}*/
		pPawn->SetActorLocation(FVector(0, 0, 0));
		pPawn->SetActorRotation(FRotator(0, 1, 0));
		vecOrientation = FIntVector(1, 0, 0);
		if (!pAGeneralSettings->Restart(iXPos, iYPos))
		{
			return ErrorState("Restart failed");
		}
		FVector vecCellLoc = pAGeneralSettings->GetClickActor(iXPos, iYPos)->GetActorLocation();
		pPawn->SetBase(nullptr);
		pPawn->SetActorLocation(FVector(vecCellLoc.X, vecCellLoc.Y, vecCellLoc.Z + 100));
		//pPawn->Restart();
		

        return g_ssGame; // ******************************* State Finished ********************************
    }
    return "";
}

FName ACMCharacter::TickStateGame(int iTickType)
{
	if (ACMachine::TICK_StateStarted == iTickType)
	{
	}
	else if (ACMachine::TICK_StateNormal == iTickType)
	{
		FName sNextState = NextNavigationState();
		if (!sNextState.IsNone())
		{
			return sNextState; // ******************************* State Finished ********************************
		}
	}
	return "";
}

FName ACMCharacter::TickStateMoving(int iTickType)
{
	if (ACMachine::TICK_StateStarted == iTickType)
	{
		if (!CanMove(vecDesiredOrientation))
		{
			if (vecOrientation != vecDesiredOrientation)
			{
				pPawn->GetMovementComponent()->StopMovementImmediately();
				return g_ssRotation; // ******************************* State Finished ********************************
			}
			else if (IsOnLog())
			{
				return g_ssLogSail; // ******************************* State Finished ********************************
			}
			return g_ssGame; // ******************************* State Finished ********************************
		}
		if (IsOnLog() && (vecDesiredOrientation.Y == 1 || vecDesiredOrientation.Y == -1))
		{
			return g_ssLogMoving; // ******************************* State Finished ********************************
		}
		vecOrientation = vecDesiredOrientation;
		if (pAGeneralSettings->IsSimpleControl())
		{
			pPawn->GetMovementComponent()->StopMovementImmediately();
			pPawn->GetCharacterMovement()->JumpZVelocity = pPawn->SmallJumpZVelocity;
			pPawn->GetCharacterMovement()->GravityScale = pPawn->SmallJumpGravityScale;
			float fFinalYaw = 0;
			if (vecOrientation.X == -1)
			{
				fFinalYaw = 180;
			}
			FRotator rot = pPawn->GetActorRotation();
			rot.Yaw = fFinalYaw;
			pPawn->SetActorRotation(rot);
			pPawn->Jump();
			pAGeneralSettings->GetClickActor(iXPos, iYPos)->AnimLandingEnd();
		}
		else
		{
			pAGeneralSettings->GetClickActor(iXPos, iYPos)->AnimLandingEnd();
		}
	}
	else if (ACMachine::TICK_StateNormal == iTickType)
	{
		AClickActor *pNextClickActor = pAGeneralSettings->GetClickActor(iXPos + vecOrientation.X, iYPos + vecOrientation.Y);
		FVector vecBlockLoc = pNextClickActor->GetActorLocation();
		if (pNextClickActor->GetGroundType() != AClickActor::eGroundType_Water || !LogInteraction())
		{
			AdjustPosition(vecBlockLoc, 300);
		}
		FVector vecLoc = pPawn->GetActorLocation();
		//if (pNextClickActor-> vecBlockLoc.X*vecOrientation.X - pAGeneralSettings->GetTileSize()/2 <= vecLoc.X*vecOrientation.X)
		

		if (vecBlockLoc.X*vecOrientation.X <= vecLoc.X*vecOrientation.X && vecBlockLoc.Y*vecOrientation.Y <= vecLoc.Y*vecOrientation.Y)
		{
			if (pNextClickActor->GetGroundType() == AClickActor::eGroundType_Water)
			{
				int32 iXPosTmp, iYPosTmp;
				pAGeneralSettings->GetTilePosition(vecLoc, iXPosTmp, iYPosTmp);
				//check(iXPosTmp == iXPos + vecOrientation.X);
				vecLoc.X = vecBlockLoc.X;
				pPawn->SetActorLocation(vecLoc);
				ChangePosition(iXPosTmp, iYPosTmp);
			}
			else
			{
				vecLoc.X = vecBlockLoc.X;
				vecLoc.Y = vecBlockLoc.Y;
				pPawn->SetActorLocation(vecLoc);
				ChangePosition(iXPos + vecOrientation.X, iYPos + vecOrientation.Y);
			}
			
			pAGeneralSettings->GetClickActor(iXPos, iYPos)->AnimLanding();
			if (pAGeneralSettings->GetClickActor(iXPos, iYPos)->GetGroundType() == AClickActor::eGroundType_Water && !IsOnLog())
			{
				return g_ssWaterDrown; // ******************************* State Finished ********************************
			}
			FName sNextState = NextNavigationState();
			if (!sNextState.IsNone())
			{
				if (vecDesiredOrientation != vecOrientation || sNextState != GetCurrentStateName())
				{
					pPawn->GetMovementComponent()->StopMovementImmediately();
					//APlayerController *pPlayerController = UGameplayStatics::GetPlayerController(pOwnerActor->GetWorld(), 0);
					//pPlayerController->ClientMessage("StopMovementImmediately");
				}
				return sNextState; // ******************************* State Finished ********************************
			}
			else
			{
				pPawn->GetMovementComponent()->StopMovementImmediately();
				if (pAGeneralSettings->GetClickActor(iXPos, iYPos)->GetGroundType() == AClickActor::eGroundType_Water)
				{
					return g_ssLogSail; // ******************************* State Finished ********************************
				}
				return g_ssGame;  // ******************************* State Finished ********************************
			}
		}
		else
		{
			if (pAGeneralSettings->IsSimpleControl())
			{
				vecLoc.X = AdjustValue(vecLoc.X, vecBlockLoc.X, pPawn->SmallJumpLinearSpeed);
				vecLoc.Y = AdjustValue(vecLoc.Y, vecBlockLoc.Y, pPawn->SmallJumpLinearSpeed);
				pPawn->SetActorLocation(vecLoc);
			}
			else
			{
				pPawn->AddMovementInput(FVector(vecOrientation.X, vecOrientation.Y, 0.f), 1);
			}
		}
	}
	else if (TICK_StateFinished == iTickType)
	{
	}
	return "";
}

FName ACMCharacter::TickStateRotation(int iTickType)
{
	if (ACMachine::TICK_StateStarted == iTickType)
	{
		vecOrientation = vecDesiredOrientation;
		//pPawn->GetMovementComponent()->StopMovementImmediately();
	}
	else if (ACMachine::TICK_StateNormal == iTickType)
	{
		float fFinalYaw = 0;
		if (-1 == vecOrientation.X)
		{
			fFinalYaw = 180;
		}
		else if (1 == vecOrientation.Y)
		{
			fFinalYaw = 90;
		}
		else if (-1 == vecOrientation.Y)
		{
			fFinalYaw = -90;
		}
		FRotator rot = pPawn->GetActorRotation();
		float fCurrentYaw = rot.Yaw;
		if (fFinalYaw - fCurrentYaw > 180)
		{
			fFinalYaw -= 360;
		}
		fCurrentYaw = AdjustValue(fCurrentYaw, fFinalYaw, pPawn->RotationSpeed);
		rot.Yaw = fCurrentYaw;
		pPawn->SetActorRotation(rot);
		if (fFinalYaw == fCurrentYaw)
		{
			if (IsLastState(g_ssRotateBeforeJump))
			{
				return g_ssJump; // ******************************* State Finished ********************************
			}
			FName sNextState = NextNavigationState();
			if (!sNextState.IsNone())
			{
				return sNextState; // ******************************* State Finished ********************************
			}
			else if (IsOnLog())
			{
				return g_ssLogSail;
			}
			else
			{
				return g_ssGame; // ******************************* State Finished ********************************
			}
		}
	}
	return "";
}

FName ACMCharacter::TickStateRotateBeforeJump(int iTickType)
{
	if (ACMachine::TICK_StateStarted == iTickType)
	{
		vecOrientation = vecDesiredOrientation;
		return g_ssRotation;
	}
	return ""; // ******************************* State Finished ********************************
}

FName ACMCharacter::TickStateJump(int iTickType)
{
	if (ACMachine::TICK_StateStarted == iTickType)
	{
		pPawn->GetMovementComponent()->StopMovementImmediately();
		pPawn->GetCharacterMovement()->JumpZVelocity = pPawn->JumpZVelocity;
		pPawn->GetCharacterMovement()->GravityScale = pPawn->JumpGravityScale;
		pPawn->Jump();
		if (!CanJump(vecOrientation))
		{
			return g_ssWaitFalling; // ******************************* State Finished ********************************
		}
		pAGeneralSettings->GetClickActor(iXPos, iYPos)->AnimLandingEnd();
		ChangePosition(iXPos + vecOrientation.X, iYPos + vecOrientation.Y);
	}
	else if (ACMachine::TICK_StateNormal == iTickType)
	{
		FVector vecBlockLoc = pAGeneralSettings->GetClickActor(iXPos, iYPos)->GetActorLocation();
		FVector vecLoc = pPawn->GetActorLocation();
		vecLoc.X = AdjustValue(vecLoc.X, vecBlockLoc.X, pPawn->JumpLinearSpeed);
		vecLoc.Y = AdjustValue(vecLoc.Y, vecBlockLoc.Y, 2*pPawn->JumpLinearSpeed);
		pPawn->SetActorLocation(vecLoc);
		if (vecLoc.X == vecBlockLoc.X && vecLoc.Y == vecBlockLoc.Y)
		{
			return g_ssJump2; // ******************************* State Finished ********************************
		}
	}
	return "";
}

FName ACMCharacter::TickStateJump2(int iTickType)
{
	if (ACMachine::TICK_StateStarted == iTickType)
	{
		if (!CanMove(vecOrientation))
		{
			if (pAGeneralSettings->GetClickActor(iXPos, iYPos)->GetGroundType() == AClickActor::eGroundType_Water)
			{
				return g_ssWaterDrown; // ******************************* State Finished ********************************
			}
			return g_ssWaitFalling; // ******************************* State Finished ********************************
		}
		ChangePosition(iXPos + vecOrientation.X, iYPos + vecOrientation.Y);
	}
	else if (ACMachine::TICK_StateNormal == iTickType)
	{
		FVector vecBlockLoc = pAGeneralSettings->GetClickActor(iXPos, iYPos)->GetActorLocation();
		FVector vecLoc = pPawn->GetActorLocation();
		vecLoc.X = AdjustValue(vecLoc.X, vecBlockLoc.X, pPawn->JumpLinearSpeed);
		vecLoc.Y = AdjustValue(vecLoc.Y, vecBlockLoc.Y, pPawn->JumpLinearSpeed);
		pPawn->SetActorLocation(vecLoc);
		if (vecLoc.X == vecBlockLoc.X && IsOnLog())
		{
			return g_ssLogSail; // ******************************* State Finished ********************************
		}
		if (vecLoc.X == vecBlockLoc.X && vecLoc.Y == vecBlockLoc.Y)
		{
			if (pAGeneralSettings->GetClickActor(iXPos, iYPos)->GetGroundType() == AClickActor::eGroundType_Water)
			{
				return g_ssWaterDrown; // ******************************* State Finished ********************************
			}
			if (!pPawn->GetCharacterMovement()->IsFalling())
			{
				pAGeneralSettings->GetClickActor(iXPos, iYPos)->AnimLanding();
				FName sNextState = NextNavigationState();
				if (!sNextState.IsNone())
				{
					return sNextState; // ******************************* State Finished ********************************
				}
				else
				{
					return g_ssGame; // ******************************* State Finished ********************************
				}
			}
		}
	}
	return "";
}

FName ACMCharacter::TickStateWaitFalling(int iTickType)
{
	if (ACMachine::TICK_StateStarted == iTickType)
	{
	}
	else if (ACMachine::TICK_StateNormal == iTickType)
	{
		if (!pPawn->GetCharacterMovement()->IsFalling())
		{
			if (IsOnLog())
			{
				return g_ssLogSail; // ******************************* State Finished ********************************
			}
			FName sNextState = NextNavigationState();
			if (!sNextState.IsNone())
			{
				return sNextState; // ******************************* State Finished ********************************
			}
			else
			{
				return g_ssGame; // ******************************* State Finished ********************************
			}
		}
	}
	return "";
}

FName ACMCharacter::TickStateWaterDrown(int iTickType)
{
	if (ACMachine::TICK_StateStarted == iTickType)
	{
		pPawn->GetMovementComponent()->StopMovementImmediately();
		FVector vecBlockLoc = pAGeneralSettings->GetClickActor(iXPos, iYPos)->GetActorLocation();
		FVector vecLoc = pPawn->GetActorLocation();
		vecLoc.X = vecBlockLoc.X;
		vecLoc.Y = vecBlockLoc.Y;
		pPawn->SetActorLocation(vecLoc);
		fWaterDrawnTime = 0;
	}
	else if (ACMachine::TICK_StateNormal == iTickType)
	{
		fWaterDrawnTime += pOwnerActor->GetWorld()->GetDeltaSeconds();
		if (IsOnLog())
		{
			return g_ssLogSail; // ******************************* State Finished ********************************
		}
		else
		{
			UPrimitiveComponent *pComponent = pPawn->GetMovementBase();
			if (fWaterDrawnTime > 5 && pComponent != nullptr)
			{
				AActor *pActor = pComponent->GetOwner();
				ECollisionChannel eChannel = pComponent->GetCollisionObjectType();
				check(false);
			}
		}
	}
	return "";
}

FName ACMCharacter::TickStateLogSail(int iTickType)
{
	if (ACMachine::TICK_StateStarted == iTickType)
	{
		UPrimitiveComponent *pComponent = pPawn->GetMovementBase();
		pStateLogSail_LogActor = pComponent->GetOwner();
		FVector vecOrigin, vecBoxExtent;
		pCurrentLogActor->GetActorBounds(false, vecOrigin, vecBoxExtent);
		float fDiff = (pPawn->GetActorLocation().Y - pStateLogSail_LogActor->GetActorLocation().Y)/pAGeneralSettings->GetTileSize();
		if (0 == (int)vecBoxExtent.Y % 100) // vecBoxExtent.Y is 50 for 1x log, 100 for 2x log, 150 for 3x log
		{
			fDiff = roundf(fDiff + 0.5f) - 0.5f;
		}
		else
		{
			fDiff = roundf(fDiff);
		}
		fStateLogSail_DesiredY = fDiff*pAGeneralSettings->GetTileSize();
		int tt = 0;
	}
	if (ACMachine::TICK_StateNormal == iTickType)
	{
		int32 iXPosTmp, iYPosTmp;
		if (!pAGeneralSettings->GetTilePosition(pPawn->GetActorLocation(), iXPosTmp, iYPosTmp))
		{
			//SetRestart(); // ******************************* State Finished ********************************
			return "";
		}
		if (AClickActor::eGroundType_Block != pAGeneralSettings->GetClickActor(iXPosTmp, iYPosTmp)->GetGroundType())
		{
			ChangePosition(iXPosTmp, iYPosTmp);

			FVector vecLoc = pPawn->GetActorLocation();
			FVector vecLogLoc = pStateLogSail_LogActor->GetActorLocation();
			vecLoc.Y = AdjustValue(vecLoc.Y, vecLogLoc.Y + fStateLogSail_DesiredY, 200);
			pPawn->SetActorLocation(vecLoc);
		}

		if (!IsOnLog())
		{
			return g_ssWaterDrown; // ******************************* State Finished ********************************
		}
		FName sNextState = NextNavigationState();
		if (!sNextState.IsNone())
		{
			if (vecDesiredOrientation != vecOrientation || sNextState != g_ssMoving)
			{
				pPawn->GetMovementComponent()->StopMovementImmediately();
				//APlayerController *pPlayerController = UGameplayStatics::GetPlayerController(pOwnerActor->GetWorld(), 0);
				//pPlayerController->ClientMessage("StopMovementImmediately");
			}
			return sNextState; // ******************************* State Finished ********************************
		}
	}
	return "";
}

FName ACMCharacter::TickStateLogMoving(int iTickType)
{
	if (ACMachine::TICK_StateStarted == iTickType)
	{
		pCurrentLogYLocation += vecDesiredOrientation.Y*pAGeneralSettings->GetTileSize();
		vecOrientation = vecDesiredOrientation;
	}
	if (ACMachine::TICK_StateNormal == iTickType)
	{
		/*UPrimitiveComponent *pComponent = pOwnerActor->GetMovementBase();
		if (pComponent == nullptr)
		{
			return g_ssWaterDrown; // ******************************* State Finished ********************************
		}
		UClass *pMovementBaseClass = pComponent->GetOwner()->GetClass();
		if (pMovementBaseClass != pGameField->Log)
		{
			return g_ssWaterDrown; // ******************************* State Finished ********************************
		}
		if (pCurrentLogActor != pComponent->GetOwner())
		{
			return g_ssLogSail; // ******************************* State Finished ********************************
		}*/

		int32 iXPosTmp, iYPosTmp;
		if (!pAGeneralSettings->GetTilePosition(pPawn->GetActorLocation(), iXPosTmp, iYPosTmp))
		{
			SetRestart(); // ******************************* State Finished ********************************
			return "";
		}
		if (AClickActor::eGroundType_Block == pAGeneralSettings->GetClickActor(iXPosTmp, iYPosTmp)->GetGroundType())
		{
			return "";
		}
		ChangePosition(iXPosTmp, iYPosTmp);

		float fDiff = pPawn->GetActorLocation().Y - pCurrentLogActor->GetActorLocation().Y;
		if (fDiff*vecOrientation.Y >= pCurrentLogYLocation*vecOrientation.Y)
		{
			FName sNextState = NextNavigationState();
			if (!sNextState.IsNone())
			{
				if (vecDesiredOrientation != vecOrientation)
				{
					pPawn->GetMovementComponent()->StopMovementImmediately();
				}
				return sNextState; // ******************************* State Finished ********************************
			}
			else
			{
				pPawn->GetMovementComponent()->StopMovementImmediately();
				if (IsOnLog())
				{
					return g_ssLogSail; // ******************************* State Finished ********************************
				}
				return g_ssWaterDrown; // ******************************* State Finished ********************************
			}
		}
		else
		{
			pPawn->AddMovementInput(FVector(vecOrientation.X, vecOrientation.Y, 0.f), 1);
		}
	}
	return "";
}
