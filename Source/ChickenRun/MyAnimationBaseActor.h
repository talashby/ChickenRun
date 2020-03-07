// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/StaticMeshActor.h"
#include "MyAnimationBaseActor.generated.h"

/**
 * 
 */
UCLASS()
class CHICKENRUN_API AMyAnimationBaseActor : public AStaticMeshActor
{
	GENERATED_BODY()
public:
	AMyAnimationBaseActor();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** 0, 90 or 180. 0 - no random */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CustomParameters", meta = (UIMin = "0", UIMax = "180"))
	uint8 OrientationRandomStep;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CustomParameters")
	bool ScaleRandom;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CustomParameters")
	uint8 ChanceToAppear;	
	
	/** Dummy root component */
	UPROPERTY(Category = Grid, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* RootSceneComponent;
	
	
	
};
