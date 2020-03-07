// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/PlayerController.h"
#include "ChickenRunPlayerController.generated.h"

UCLASS()
class AChickenRunPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AChickenRunPlayerController();

protected:
	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;

	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	void MoveRight(float Value);
	// End PlayerController interface

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Navigate player to the current mouse cursor location. */
	void MoveToMouseCursor();

	/** Navigate player to the current touch location. */
	void MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location);
	
	/** Navigate player to the given world location. */
	void SetNewMoveDestination(const FVector DestLocation);

	virtual bool InputKey(FKey Key, EInputEvent EventType, float AmountDepressed, bool bGamepad) override;
	void TouchPressed(const ETouchIndex::Type FingerIndex, const FVector Location);
	void TouchReleased(const ETouchIndex::Type FingerIndex, const FVector Location);

	//void LMBPressed(const FVector Location);
	//void LMBReleased(const FVector Location);

	void ClickPressed(const FVector ClickLocation);
	void ClickReleased(const FVector ClickLocation);

	/** Input handlers for SetDestination action. */
	void OnSetDestinationPressed();
	void OnSetDestinationReleased();

private:
	FVector vecPressed;
	float fPressedTime;
	class AGeneralSettings *pAGeneralSettings;
};


