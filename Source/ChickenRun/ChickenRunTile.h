// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/StaticMeshActor.h"
#include "ChickenRunTile.generated.h"

/**
 * 
 */
UCLASS()
class CHICKENRUN_API AChickenRunTile : public AStaticMeshActor
{
	GENERATED_BODY()

public:

	UFUNCTION(Category = "Animation", BlueprintImplementableEvent, BlueprintCallable)
	void AnimLanding();

	UFUNCTION(Category = "Animation", BlueprintImplementableEvent, BlueprintCallable)
	void AnimLandingEnd();
	
};
