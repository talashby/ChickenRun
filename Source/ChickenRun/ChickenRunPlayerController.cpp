// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "ChickenRun.h"
#include "ChickenRunPlayerController.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "ChickenRunCharacter.h"
#include "ACMGeneral.h"
#include "ACMCharacter.h"
#include "GeneralSettings.h"
#include "ChickenRunGameMode.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

AChickenRunPlayerController::AChickenRunPlayerController()
{
	bEnableClickEvents = true;
	bEnableTouchEvents = true;
	bShowMouseCursor = true;
	DefaultClickTraceChannel = ECollisionChannel::ECC_GameTraceChannel1;
	fPressedTime = 0;
	//DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void AChickenRunPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	// keep updating the destination every tick while desired
	if (bMoveToMouseCursor)
	{
		MoveToMouseCursor();
	}
}

void AChickenRunPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	//InputComponent->BindAction("SetDestination", IE_Pressed, this, &AChickenRunPlayerController::OnSetDestinationPressed);
	//InputComponent->BindAction("SetDestination", IE_Released, this, &AChickenRunPlayerController::OnSetDestinationReleased);

	// support touch devices 
	InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AChickenRunPlayerController::TouchPressed);
	InputComponent->BindTouch(EInputEvent::IE_Released, this, &AChickenRunPlayerController::TouchReleased);

	if (!MyHelper::GetActorFromScene(GetWorld(), "GeneralSettings", pAGeneralSettings))
	{
		UE_LOG(LogGeneral, Error, TEXT("AChickenRunPlayerController::SetupInputComponent : GeneralSettings not found"));
	}
}

void AChickenRunPlayerController::MoveToMouseCursor()
{/*
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		if (AChickenRunCharacter* MyPawn = Cast<AChickenRunCharacter>(GetPawn()))
		{
			if (MyPawn->GetCursorToWorld())
			{
				UNavigationSystem::SimpleMoveToLocation(this, MyPawn->GetCursorToWorld()->GetComponentLocation());
			}
		}
	}
	else
	{
		// Trace to see what is under the mouse cursor
		FHitResult Hit;
		GetHitResultUnderCursor(ECC_Visibility, false, Hit);

		if (Hit.bBlockingHit)
		{
			// We hit something, move there
			SetNewMoveDestination(Hit.ImpactPoint);
		}
	}*/
}

void AChickenRunPlayerController::MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	FVector2D ScreenSpaceLocation(Location);

	// Trace to see what is under the touch location
	FHitResult HitResult;
	GetHitResultAtScreenPosition(ScreenSpaceLocation, CurrentClickTraceChannel, true, HitResult);
	if (HitResult.bBlockingHit)
	{
		// We hit something, move there
		SetNewMoveDestination(HitResult.ImpactPoint);
	}
}

void AChickenRunPlayerController::SetNewMoveDestination(const FVector DestLocation)
{
	APawn* const MyPawn = GetPawn();
	if (MyPawn)
	{
		//UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent(GetWorld());
		float const Distance = FVector::Dist(DestLocation, MyPawn->GetActorLocation());

		// We need to issue move command only if far enough in order for walk animation to play correctly
		/* if (NavSys && (Distance > 120.0f))
		{
			NavSys->SimpleMoveToLocation(this, DestLocation);
		} */
		if (Distance > 120.0f)
		{
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, DestLocation);
		}
	}
}

bool AChickenRunPlayerController::InputKey(FKey Key, EInputEvent EventType, float AmountDepressed, bool bGamepad)
{
	if ("LeftMouseButton" == Key.GetFName())
	{
		float fLocationX, fLocationY;
		GetMousePosition(fLocationX, fLocationY);
		if (EInputEvent::IE_Pressed == EventType)
		{
			ClickPressed(FVector(fLocationX, fLocationY, 0));
		}
		else if (EInputEvent::IE_Released == EventType)
		{
			ClickReleased(FVector(fLocationX, fLocationY, 0));
		}
	}
	return Super::InputKey(Key, EventType, AmountDepressed, bGamepad);
}

void AChickenRunPlayerController::TouchPressed(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	ClickPressed(Location);
}

void AChickenRunPlayerController::TouchReleased(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	ClickReleased(Location);
}

void AChickenRunPlayerController::ClickPressed(const FVector ClickLocation)
{
	vecPressed = ClickLocation;
	fPressedTime = UGameplayStatics::GetRealTimeSeconds(GetWorld());
}

void AChickenRunPlayerController::ClickReleased(const FVector vecClickLocation)
{
/*
	if (GEngine)
	{
		FVector vecDrag = vecPressed - Location;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::FromInt(int(vecDrag.Size())));
	}*/
	float realtimeSeconds = UGameplayStatics::GetRealTimeSeconds(GetWorld()) - fPressedTime;
	if (realtimeSeconds < 1)
	{
		int iJumpX = 0;
		{
			float fDragSizeX = vecPressed.X - vecClickLocation.X;
			if (fDragSizeX > pAGeneralSettings->DragStep && fDragSizeX < 500)
			{
				iJumpX = 1;
			}
			else if (fDragSizeX < -pAGeneralSettings->DragStep && fDragSizeX > -500)
			{
				iJumpX = -1;
			}
		}
		int iJumpY = 0;
		{
			float fDragSizeY = vecPressed.Y - vecClickLocation.Y;
			if (fDragSizeY > pAGeneralSettings->DragStep && fDragSizeY < 500)
			{
				iJumpY = 1;
			}
			else if (fDragSizeY < -pAGeneralSettings->DragStep && fDragSizeY > -500)
			{
				iJumpY = -1;
			}
		}
		if (iJumpX != 0 || iJumpY != 0)
		{
			/*FConstPawnIterator it = GetWorld()->GetPawnIterator();
			AChickenRunCharacter *pPawn = Cast<AChickenRunCharacter>(it->Get());*/
			AGameModeBase *pGameMode = GetWorld()->GetAuthGameMode();
			AChickenRunGameMode *pCRGameMode = Cast<AChickenRunGameMode>(pGameMode);
			SP_ACMGeneral spACMGeneral = pCRGameMode->GetACMGeneral();
			if (nullptr != spACMGeneral.Get())
			{
				SP_ACMCharacter spACMCharacter = spACMGeneral->GetACMCharacter();
				if (nullptr != spACMCharacter.Get())
				{
					if (1 == iJumpX)
					{
						spACMCharacter->JumpBackward();
					}
					else if(-1 == iJumpX)
					{
						spACMCharacter->JumpForward();
					}
					else if (1 == iJumpY)
					{
						spACMCharacter->JumpLeft();
					}
					else if (-1 == iJumpY)
					{
						spACMCharacter->JumpRight();
					}
				}
			}
		}
		/*else
		{
			FConstPawnIterator it = GetWorld()->GetPawnIterator();
			AChickenRunCharacter *pPawn = Cast<AChickenRunCharacter>(it->Get());
			FVector2D vecScreenLocation;
			bool bSuccess = ProjectWorldLocationToScreen(pPawn->GetActorLocation(), vecScreenLocation);
			if (bSuccess)
			{
				vecScreenLocation += FVector2D(100, 27); // magic correction
				FVector vecScreenTrianglePoint = FVector(vecScreenLocation.X, vecScreenLocation.Y, 0);
				FVector vecBaryCentric = FMath::ComputeBaryCentric2D(vecClickLocation, vecScreenTrianglePoint, vecScreenTrianglePoint + FVector(10000, -10000, 0), vecScreenTrianglePoint + FVector(10000, 10000, 0));
				FIntVector vecMoveDirection = FIntVector(0, 0, 0);
				if (vecBaryCentric.X > 0 && vecBaryCentric.Y > 0 && vecBaryCentric.Z > 0)
				{
					vecMoveDirection = FIntVector(1, 0, 0);
				}
				else
				{
					vecBaryCentric = FMath::ComputeBaryCentric2D(vecClickLocation, vecScreenTrianglePoint, vecScreenTrianglePoint + FVector(-10000, -10000, 0), vecScreenTrianglePoint + FVector(-10000, 10000, 0));
					if (vecBaryCentric.X > 0 && vecBaryCentric.Y > 0 && vecBaryCentric.Z > 0)
					{
						vecMoveDirection = FIntVector(-1, 0, 0);
					}
					else
					{
						vecBaryCentric = FMath::ComputeBaryCentric2D(vecClickLocation, vecScreenTrianglePoint, vecScreenTrianglePoint + FVector(-10000, -10000, 0), vecScreenTrianglePoint + FVector(10000, -10000, 0));
						if (vecBaryCentric.X > 0 && vecBaryCentric.Y > 0 && vecBaryCentric.Z > 0)
						{
							vecMoveDirection = FIntVector(0, -1, 0);
						}
						else
						{
							vecBaryCentric = FMath::ComputeBaryCentric2D(vecClickLocation, vecScreenTrianglePoint, vecScreenTrianglePoint + FVector(-10000, 10000, 0), vecScreenTrianglePoint + FVector(10000, 10000, 0));
							if (vecBaryCentric.X > 0 && vecBaryCentric.Y > 0 && vecBaryCentric.Z > 0)
							{
								vecMoveDirection = FIntVector(0, 1, 0);
							}
						}
					}
				}

				SP_ACMGeneral spACMGeneral = pPawn->GetACMGeneral();
				if (nullptr != spACMGeneral.Get())
				{
					SP_ACMCharacter spACMCharacter = spACMGeneral->GetACMCharacter();
					if (nullptr != spACMCharacter.Get())
					{
						spACMCharacter->Move(vecMoveDirection);
					}
				}
			}
		}*/
	}
}

void AChickenRunPlayerController::OnSetDestinationPressed()
{
	// set flag to keep updating destination until released
	bMoveToMouseCursor = true;
}

void AChickenRunPlayerController::OnSetDestinationReleased()
{
	// clear flag to indicate we should stop updating the destination
	bMoveToMouseCursor = false;
}
