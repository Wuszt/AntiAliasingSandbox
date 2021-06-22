//Include and link appropriate libraries and headers//
#include <windows.h>
#include "MyApp.h"
#include "Tester.h"
#include <string>
#include <iostream>

bool TryToGetParams(LPSTR cmds, int outParams[4])
{
    std::string str = cmds;

    int counter = 0;
    size_t i = -1;
    do
    {
        size_t prevI = i + 1;
        i = str.find(' ', prevI);
        std::string tmp = str.substr(prevI, i - prevI);
        try
        {
            outParams[counter] = std::stoi(tmp);
            ++counter;
        }
        catch (...)
        {
            return false;
        }
    } while (i != std::string::npos);

    return counter >= 4;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    Core* core = new Core();
    const char* tmp = __FUNCSIG__;
    int params[4];
    if (!TryToGetParams(lpCmdLine, params))
    {
        params[0] = 1920;
        params[1] = 1080;
        params[2] = 1920;
        params[3] = 1080;
    }

    core->Run(hInstance, nShowCmd, params[0], params[1], params[2], params[3], "Results");
    delete core;

    return 0;
}