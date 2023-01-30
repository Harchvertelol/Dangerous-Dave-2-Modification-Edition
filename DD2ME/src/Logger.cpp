#include "Logger.h"

#include <iostream>
#include <chrono>
#include <sstream>
#include <fstream>

#include "Defines.h"

using namespace std;

Logger::Logger():
    s_PrintError(true),
    s_PrintLogicViolation(true),
    s_PrintWarning(true),
    s_PrintStandard(true),
    s_PrintInfo(true),
    s_PrintDebugInfo(true),
    s_PrintToConsole(true),
    s_PrintToFile(true),
    s_LogFileName("debug.log")
{}

Logger::~Logger()
{}

string Logger::getResultPostProcessStringEvent(LoggerEventType TypeEvent, string TextEvent, bool IsFatal)
{
    stringstream result_str;
    auto time_now = chrono::system_clock::now();
    time_t time_now_t = chrono::system_clock::to_time_t(time_now);
    string time_now_str = ctime(&time_now_t);
    result_str << "[" << time_now_str.substr(0, time_now_str.size() - 1) << "] ";
    time_now_str = result_str.str();
    if(TypeEvent == EVENT_TYPE_ERROR && s_PrintError)
    {
        result_str << "Error: " << TextEvent;
    }
    else if(TypeEvent == EVENT_TYPE_LOGIC_VIOLATION && s_PrintLogicViolation)
    {
        result_str << "Logic violation: " << TextEvent;
    }
    else if(TypeEvent == EVENT_TYPE_WARNING && s_PrintWarning)
    {
        result_str << "Warning: " << TextEvent;
    }
    else if(TypeEvent == EVENT_TYPE_STANDARD && s_PrintStandard)
    {
        result_str << "Standard: " << TextEvent;
    }
    else if(TypeEvent == EVENT_TYPE_INFO && s_PrintInfo)
    {
        result_str << "Information: " << TextEvent;
    }
    else if(TypeEvent == EVENT_TYPE_DEBUG_INFO && s_PrintDebugInfo)
    {
        result_str << "Debug info: " << TextEvent;
    }
    if(time_now_str.size() == result_str.str().size()) return "";
    if(IsFatal == true) result_str << " -> Fatal!";
    result_str << endl;
    return result_str.str();
}

void Logger::registerEventStandardConsole(string event_str)
{
    cout << event_str;
}

void Logger::registerEvent(LoggerEventType TypeEvent, string TextEvent, bool IsFatal)
{
    string event_result_str = getResultPostProcessStringEvent(TypeEvent, TextEvent, IsFatal);
    if(event_result_str != "")
    {
        if(s_PrintToConsole) registerEventStandardConsole(event_result_str);
        if(s_PrintToFile) registerEventStandardFile(event_result_str);
    }
}

void Logger::registerEventStandardFile(string event_str)
{
    ofstream s_LogFileStream(s_LogFileName, ios::app|ios::out);
    if(!s_LogFileStream)
    {
        registerEventStandardConsole(getResultPostProcessStringEvent(EVENT_TYPE_ERROR, "Open log file: " + s_LogFileName, false));
        return;
    }
    s_LogFileStream << event_str;
    s_LogFileStream.close();
}

void Logger::setPrintDebugInfo(bool value)
{
    s_PrintDebugInfo = value;
}

bool Logger::getPrintDebugInfo()
{
    return s_PrintDebugInfo;
}

void Logger::setLogFileName(string value)
{
    s_LogFileName = value;
}

string Logger::getLogFileName()
{
    return s_LogFileName;
}

void Logger::setFilePrint(bool value)
{
    s_PrintToFile = value;
}

bool Logger::getFilePrint()
{
    return s_PrintToFile;
}

void Logger::setConsolePrint(bool value)
{
    s_PrintToConsole = value;
}

bool Logger::getConsolePrint()
{
    return s_PrintToConsole;
}

void Logger::setPrintError(bool value)
{
    s_PrintError = value;
}

bool Logger::getPrintError()
{
    return s_PrintError;
}

void Logger::setPrintLogicViolation(bool value)
{
    s_PrintLogicViolation = value;
}

bool Logger::getPrintLogicViolation()
{
    return s_PrintLogicViolation;
}

void Logger::setPrintWarning(bool value)
{
    s_PrintWarning = value;
}

bool Logger::getPrintWarning()
{
    return s_PrintWarning;
}

void Logger::setPrintStandard(bool value)
{
    s_PrintStandard = value;
}
bool Logger::getPrintStandard()
{
    return s_PrintStandard;
}

void Logger::setPrintInfo(bool value)
{
    s_PrintInfo = value;
}
bool Logger::getPrintInfo()
{
    return s_PrintInfo;
}
