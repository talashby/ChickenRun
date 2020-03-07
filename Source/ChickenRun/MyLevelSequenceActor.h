// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "LevelSequenceActor.h"
#include "MyLevelSequenceActor.generated.h"

/**
 * 
 */
UCLASS()
class CHICKENRUN_API AMyLevelSequenceActor : public ALevelSequenceActor
{
	GENERATED_BODY()
	AMyLevelSequenceActor(const FObjectInitializer& Init);

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CustomParameters")
	bool DirectionRandom;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CustomParameters", meta = (UIMin = "0", UIMax = "1"))
	float StartDelayMin;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CustomParameters", meta = (UIMin = "0", UIMax = "1"))
	float StartDelayMax;
};
