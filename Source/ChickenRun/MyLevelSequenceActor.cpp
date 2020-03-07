// Fill out your copyright notice in the Description page of Project Settings.

#include "ChickenRun.h"
#include "MyLevelSequenceActor.h"




AMyLevelSequenceActor::AMyLevelSequenceActor(const FObjectInitializer& Init) : ALevelSequenceActor(Init)
{
	DirectionRandom = true;
}
