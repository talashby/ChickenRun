// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "ChickenRunCharacter.generated.h"

UCLASS(Blueprintable)
class AChickenRunCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AChickenRunCharacter();

	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	virtual void BeginPlay() override;

	UFUNCTION(Category = "Animation", BlueprintImplementableEvent, BlueprintCallable)
	void AnimBurn(int BurnInPercent);

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns CursorToWorld subobject **/
	//FORCEINLINE class UDecalComponent* GetCursorToWorld() { return CursorToWorld; }
	/** Jump linear speed */
	UPROPERTY(Category = "Character Movement: Jumping / Falling", EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "Jump Linear Speed", ClampMin = "0", UIMin = "0"))
	float JumpLinearSpeed;
	UPROPERTY(Category = "Character Movement: Jumping / Falling", EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "Jump Z Velocity", ClampMin = "0", UIMin = "0"))
	float JumpZVelocity;
	UPROPERTY(Category = "Character Movement: Jumping / Falling", EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "Jump Gravity Scale", ClampMin = "0", UIMin = "0"))
	float JumpGravityScale;
	UPROPERTY(Category = "Character Movement: Jumping / Falling", EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "Small Jump Linear Speed", ClampMin = "0", UIMin = "0"))
	float SmallJumpLinearSpeed;
	UPROPERTY(Category = "Character Movement: Jumping / Falling", EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "Small Jump Z Velocity", ClampMin = "0", UIMin = "0"))
	float SmallJumpZVelocity;
	UPROPERTY(Category = "Character Movement: Jumping / Falling", EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "Small Jump Gravity Scale", ClampMin = "0", UIMin = "0"))
	float SmallJumpGravityScale;
	UPROPERTY(Category = "Character Movement: Jumping / Falling", EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "Rotate Speed", ClampMin = "0", UIMin = "0"))
	float RotationSpeed;

//	TSubclassOf<class AActor> pLogClass; // blueprint class


private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** A decal that projects to the cursor location. */
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	//class UDecalComponent* CursorToWorld;

	UStaticMesh *pCubeMesh;
};

