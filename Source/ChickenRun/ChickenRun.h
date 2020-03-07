// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#ifndef __CHICKENRUN_H__
#define __CHICKENRUN_H__

#include <vector>
#include <list>
#include <map>
#include "EngineMinimal.h"
#include "EngineGlobals.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"
#include "SlateBasics.h"
#include "EngineUtils.h"
#include "Runtime/LevelSequence/Public/LevelSequence.h"
#include "Runtime/LevelSequence/Public/LevelSequenceActor.h"
#include "Runtime/MovieScene/Public/MovieScene.h"
#include "Engine/TriggerVolume.h"
#include "GameFramework/PlayerStart.h"
#include "Components/TextRenderComponent.h"
#include <random>

//General Log
DECLARE_LOG_CATEGORY_EXTERN(LogGeneral, Log, All);
namespace LogHelper
{
	bool CheckLogLevel(int iLogSeverityLevel);
}

namespace MyHelper
{
	// empty name for first actor
	template <typename T> bool GetActorFromScene(UWorld *pWorld, FString sActorName, T* &pOutActor)
	{
		for (TActorIterator<T> ActorItr(pWorld); ActorItr; ++ActorItr)
		{
			if (ActorItr->GetName() == sActorName || sActorName.IsEmpty())
			{
				pOutActor = *ActorItr;
				return true;
			}
		}
		return false;
	}
}

struct MT19937
{
private:
	static std::random_device rd;
	static std::mt19937 rng;
public:
	// This is equivalent to srand().
	static void seed(uint64_t new_seed = std::mt19937::default_seed)
	{
		rng.seed(new_seed);
	}

	// This is equivalent to rand().
	static int32 get(int32 iRandMax);
};

#endif
