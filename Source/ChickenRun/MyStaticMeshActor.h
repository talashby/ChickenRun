// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/StaticMeshActor.h"
#include "MyStaticMeshActor.generated.h"

/**
 * 
 */
UCLASS()
class CHICKENRUN_API AMyStaticMeshActor : public AStaticMeshActor
{
	GENERATED_BODY()

public:
	AMyStaticMeshActor();
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CustomParameters")
	bool OrientationRandom;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CustomParameters")
	bool ScaleRandom;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CustomParameters")
	uint8 ChanceToAppear;

	class AClickActor *pOwnerTile;
};
