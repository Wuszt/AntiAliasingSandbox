//Include and link appropriate libraries and headers//
#include <windows.h>
#include "MyApp.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    Core* core = new MyApp();
    core->Run(hInstance, nShowCmd, 1024, 1024);
    delete core;

    return 0;
}


