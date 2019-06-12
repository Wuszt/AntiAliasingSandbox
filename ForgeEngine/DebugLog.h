#pragma once

#define VISUAL_CONSOLE

#include <DirectXMath.h>

#ifdef VISUAL_CONSOLE
#include <sstream>
#include <windows.h>
#else
#include <string>
#include <iostream>
#endif


struct DirectX::XMFLOAT3;

class DebugLog
{
public:
    DebugLog() = delete;
    ~DebugLog() = delete;

    template<class T>
    static void Print(const T& val)
    {
        if (!s_isInitialized)
            Initialize();

#ifdef VISUAL_CONSOLE
        PrintToVisualConsole(val);
#else
        PrintToOutsideConsole
#endif
    }

private:
    static void Initialize();
    static bool s_isInitialized;

    static void InitializeOutsideConsole();

    template<class T>
    static void PrintToVisualConsole(const T& val)
    {
        std::ostringstream ss;
        ss << val << "\n";

        OutputDebugString(ss.str().c_str());
    }

    template<class T>
    static void PrintToOutsideConsole(const T& val)
    {
        std::cout << val << "\n";
    }
};

std::ostream& operator<< (std::ostream& os, const DirectX::XMFLOAT3& vec);

