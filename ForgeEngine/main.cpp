//Include and link appropriate libraries and headers//
#include <windows.h>
#include "Core.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    Core* core = new Core(hInstance, nShowCmd, 1024, 1024);
    core->Run();
    delete core;

    return 0;
}


