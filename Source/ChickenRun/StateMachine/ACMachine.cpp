// Fill out your copyright notice in the Description page of Project Settings.

#include "ChickenRun.h"
#include "ACMachine.h"

static const FName g_ssStateNone("None");
static const FName g_ssStateStart("Start");
static const FName g_ssStateError("Error");

ACMachine::ACMachine(const FString &sName)
{
    //UE_LOG(YourLog, Warning, TEXT("This is a message to yourself during runtime!"));
    m_iLogSeverityLevel = ELogVerbosity::Verbose;
    m_sNameMachine = sName;
    REGISTER_ACSTATE(ACMachine, Start);
    REGISTER_ACSTATE(ACMachine, Error);

    m_sState = g_ssStateNone;
    m_fnCurrentState = (fnACStateHandler)&ACMachine::TickStateNone;
    m_bRestart = false;
    //SetNextState(g_ssStateStart);
}

void ACMachine::TickPublic()
{
    OperateStates();
    Tick();
    TickHierarchical();
}

bool ACMachine::IsInNoneState() const
{
    return IsCurrentState(g_ssStateNone);
}

bool ACMachine::IsInStartState() const
{
    return IsCurrentState(g_ssStateStart);
}

bool ACMachine::IsInErrorState() const
{
    return g_ssStateError == m_sState;
}

void ACMachine::OperateStates()
{
    FName sNextState;
    if (m_bRestart)
    {
        sNextState = g_ssStateStart;
        m_bRestart = false;
    }
    else
    {
        sNextState = (this->*(m_fnCurrentState))(TICK_StateNormal);
    }
    bool bStateChanged = SetNextState(sNextState);
    while (bStateChanged)
    {
        (this->*(m_fnCurrentState))(TICK_StateFinished);
        m_aStatesArchive.push_back(m_sNextState);
        if (m_aStatesArchive.size() > 20)
        {
            m_aStatesArchive.pop_front();
        }
        if (LogHelper::CheckLogLevel(m_iLogSeverityLevel))
        {
            FString str = "Finished state: '" + m_sState.ToString() + "'. Start state: '" + m_sNextState.ToString() + "'";
            WriteLogMessage(m_iLogSeverityLevel, str);
        }
        m_sLastState = m_sState;
        m_sState = m_sNextState;
        m_fnCurrentState = m_fnNextState;
        BeforeTickStateStartedHierarchical();
        BeforeTickStateStarted();
        FName sNextState2 = (this->*(m_fnCurrentState))(TICK_StateStarted);
        bStateChanged = SetNextState(sNextState2);
    }
}

bool ACMachine::IsCurrentState(const FName &sStateName) const
{
    return m_sState == sStateName;
}

bool ACMachine::IsLastState(const FName &sStateName) const
{
    return m_sLastState == sStateName;
}

bool ACMachine::IsNextState(const FName &sStateName) const
{
    return m_sNextState == sStateName;
}

void ACMachine::SetNextErrorState(const FString& sMessage/* = ""*/)
{
    WriteLogMessage(ELogVerbosity::Error, FString::Printf(TEXT("Error. Switching to Error state at state \"%s\" with message \"%s\""), *m_sState.ToString(), *sMessage));
    SetNextState(g_ssStateError);
}

FName ACMachine::TickStateNone(int iTickType)
{
    return g_ssStateStart;
}

FName ACMachine::GetStateStartName() const
{
    return g_ssStateStart;
}

FName ACMachine::GetCurrentStateName() const
{
	return m_sState;
}

FName ACMachine::ErrorState(const FString &sMessage)
{
    WriteLogMessage(ELogVerbosity::Error, FString::Printf(TEXT("Error. Switching to Error state at state \"%s\" with message \"%s\""), *m_sState.ToString(), *sMessage));
    WriteStatesArchiveToErrorLog();
    m_sErrorMessage = sMessage;
    return g_ssStateError;
}

void ACMachine::WriteStatesArchiveToErrorLog() const
{
    FString str = "States archive: ";
    std::list<FName>::const_iterator it = m_aStatesArchive.begin();
    while (it != m_aStatesArchive.end())
    {
        str += it->ToString();
        ++it;
        if (it != m_aStatesArchive.end())
        {
            str += " -> ";
        }
    }
    WriteLogMessage(ELogVerbosity::Error, str);
}

FString ACMachine::GetLogPrefix() const
{
    FString prefix = "ACMachine : " + m_sNameMachine + "(" + m_sState.ToString() + ") : ";
    return prefix;
}

FString ACMachine::GetName() const
{
    return m_sNameMachine;
}

void ACMachine::SetName(const FString &sName)
{
    m_sNameMachine = sName;
}

void ACMachine::SetStateExternalSignal(int iExternalSignal)
{
    WriteLogMessage(m_iLogSeverityLevel, FString::Printf(TEXT("External signal was set : %d"), iExternalSignal));
    m_iStateExternalSignal = iExternalSignal;
}

bool ACMachine::IsStateExternalSignal(int iExternalSignal) const
{
    return iExternalSignal == m_iStateExternalSignal;
}

void ACMachine::SetLogSeverity(int iLogSeverityLevel)
{
    m_iLogSeverityLevel = iLogSeverityLevel;
}

bool ACMachine::RegisterState(const FName& sStateName, fnACStateHandler pStateHandler)
{
    return m_aStateMap.insert(tStateMap::value_type(sStateName, pStateHandler)).second;
}

bool ACMachine::UnregisterState(const FName& sStateName)
{
    return m_aStateMap.erase(sStateName) == 1;
}

bool ACMachine::SetNextState(const FName& sStateTo)
{
    if (!sStateTo.IsNone())
    {
        tStateMap::const_iterator itStateMap = m_aStateMap.find(sStateTo);
        if (itStateMap != m_aStateMap.end())
        {
            m_fnNextState = itStateMap->second;
            m_sNextState = sStateTo;
        }
        else
        {
            SetNextErrorState(FString::Printf(TEXT("Failed to switch to state \"%s\""), *sStateTo.ToString()));
            m_sNextState = g_ssStateError;
        }
        return true;
    }
    return false;
}

void ACMachine::WriteLogMessage(int iLevel, const FString &msg) const
{
	FMsg::Logf("", 0, TEXT(""), (ELogVerbosity::Type)iLevel, TEXT("%s"), *(GetLogPrefix() + msg));
}

void ACMachine::BeforeTickStateStartedHierarchical()
{
    m_iStateExternalSignal = EXTERNAL_SIGNAL_None;
}

void ACMachine::Restart()
{
    m_bRestart = true;
    OperateStates();
}

void ACMachine::SetRestart()
{
	m_bRestart = true;
}

