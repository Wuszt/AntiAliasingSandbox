#pragma once
#include <windows.h>

class Window
{
public:
    Window(const HINSTANCE& hInstance,const int& ShowWnd,const int& width,const int& height,const bool& windowed);
    ~Window();

    void Update();

    inline HWND* GetHWND() { return &m_hwnd; }
    inline bool IsAlive() const { return m_isAlive; }

private:
    HWND m_hwnd;
    bool m_isAlive = true;
    
    inline void SetAsDead() { m_isAlive = false; }

    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

