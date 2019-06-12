#include "DebugLog.h"
#include <iostream>
#include <windows.h>

using namespace std;

bool DebugLog::s_isInitialized;

void DebugLog::InitializeOutsideConsole()
{
    AllocConsole();
    freopen("conin$", "r", stdin);
    freopen("conout$", "w", stdout);
    freopen("conout$", "w", stderr);
}

void DebugLog::Initialize()
{
    s_isInitialized = true;

#ifndef VISUAL_CONSOLE
    InitializeOutsideConsole();
#endif

    Print("Debug Console");
}

std::ostream& operator<<(std::ostream& os, const DirectX::XMFLOAT3& vec)
{
    os << "[" + to_string(vec.x) + " , " + to_string(vec.y) + " , " + to_string(vec.z) + "]";
    return os;
}
