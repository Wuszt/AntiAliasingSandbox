#pragma once
#include <windows.h>
#include <unordered_set>

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

    inline UINT GetResolutionWidth() const { return m_resWidth; }
    inline UINT GetResolutionHeight() const { return m_resHeight; }

    void SetResolution(int width, int height);

    void AddResizeListener(void(*callback)(const int&, const int&));
    void RemoveResizeListener(void(*callback)(const int&, const int&));

    void AddResolutionChangeListener(void(*callback)(const int&, const int&));
    void RemoveResolutionChangeListener(void(*callback)(const int&, const int&));
    inline void SetAsDead() { m_isAlive = false; }

private:
    HWND m_hwnd;
    HINSTANCE m_hInstance;
    bool m_isAlive = true;

    UINT m_width;
    UINT m_height;

    UINT m_resWidth;
    UINT m_resHeight;

    bool m_justResized = false;

    void OnResized();

    std::unordered_set<void(*)(const int&,const int&)> m_resizeListeners;
    std::unordered_set<void(*)(const int&, const int&)> m_resolutionChangeListener;
   

    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

