#pragma once
#include <windows.h>

class Window
{
public:
    Window(const HINSTANCE& hInstance,const int& ShowWnd,const int& width,const int& height,const bool& windowed);
    ~Window();

    void Update();

    inline HWND* GetHWND() { return &m_hwnd; }
    inline HINSTANCE* GetHInstance() { return &m_hInstance; }
    inline bool IsAlive() const { return m_isAlive; }

    inline UINT GetWidth() const { return m_width; }
    inline UINT GetHeight() const { return m_height; }

private:
    HWND m_hwnd;
    HINSTANCE m_hInstance;
    bool m_isAlive = true;

    UINT m_width;
    UINT m_height;
    
    inline void SetAsDead() { m_isAlive = false; }

    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

