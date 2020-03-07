// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "ChickenRun.h"


IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, ChickenRun, "ChickenRun" );

//General Log
DEFINE_LOG_CATEGORY(LogGeneral);

namespace LogHelper
{
	bool CheckLogLevel(int iLogSeverityLevel)
	{
#if NO_LOGGING
		return false;
#else
		if ((iLogSeverityLevel & ELogVerbosity::VerbosityMask) <= ELogVerbosity::COMPILED_IN_MINIMUM_VERBOSITY && (ELogVerbosity::Warning & ELogVerbosity::VerbosityMask) <= FLogCategoryLogGeneral::CompileTimeVerbosity)
		{
			return true;
		}
		return false;
#endif
	}
}

std::random_device MT19937::rd;
std::mt19937 MT19937::rng = std::mt19937(rd());

int32 MT19937::get(int32 iRandMax)
{
	std::uniform_int_distribution<int32> dist(0, iRandMax);
	return dist(rng);
}
