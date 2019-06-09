#pragma once
#include <windows.h>

class Window
{
public:
    Window(HINSTANCE hInstance, int ShowWnd, int width, int height, bool windowed);
    ~Window();

    //LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
private:
    HWND m_hwnd;

public:
    inline HWND* GetHWND() { return &m_hwnd; }
};

