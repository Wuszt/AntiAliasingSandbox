#include "Window.h"
#include <exception>

using namespace std;

LRESULT CALLBACK Window::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    Window* window;

    switch (msg)
    {
    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE) {
            DestroyWindow(hwnd);
        }
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_NCDESTROY:
        window = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
        window->SetAsDead();
        return 0;

    case WM_EXITSIZEMOVE:
        window = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
        window->m_justResized = true;

        return 0;

    case WM_WINDOWPOSCHANGING:
        window = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
        WINDOWPOS* pos = (WINDOWPOS*)lParam;

        pos->cy = (int)(9.0f / 16.0f * pos->cx);

        if (pos->cx > 0)
        {
            window->m_width = pos->cx;
            window->m_height = pos->cy;
        }

        DefWindowProc(hwnd,
            msg,
            wParam,
            lParam);

        return 0;
    }

    return DefWindowProc(hwnd,
        msg,
        wParam,
        lParam);
}

void Window::OnResized()
{
    m_justResized = false;

    for (auto& callback : m_resizeListeners)
    {
        callback(GetWidth(), GetHeight());
    }
}


Window::Window(const HINSTANCE& hInstance, const int& ShowWnd, const int& width, const int& height, const bool& windowed)
{
    m_width = width;
    m_height = height;

    m_resWidth = width;
    m_resHeight = height;

    typedef struct _WNDCLASS {
        UINT cbSize;
        UINT style;
        WNDPROC lpfnWndProc;
        int cbClsExtra;
        int cbWndExtra;
        HANDLE hInstance;
        HICON hIcon;
        HCURSOR hCursor;
        HBRUSH hbrBackground;
        LPCTSTR lpszMenuName;
        LPCTSTR lpszClassName;
    } WNDCLASS;

    WNDCLASSEX wc;

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = NULL;
    wc.cbWndExtra = NULL;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "ForgeEngine";
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc))
    {
        MessageBox(NULL, "Error registering class",
            "Error", MB_OK | MB_ICONERROR);
        throw std::exception("Error registering window class");
    }

    m_hwnd = CreateWindowEx(
        0,
        "ForgeEngine",
        "ForgeEngine",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        width, height,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );

    SetWindowLongPtrA(m_hwnd, GWLP_USERDATA, (LONG_PTR)this);

    if (!m_hwnd)
    {
        MessageBox(nullptr, "Error creating window",
            "Error", MB_OK | MB_ICONERROR);
        throw std::exception("Error creating window");
    }

    ShowWindow(m_hwnd, ShowWnd);
    UpdateWindow(m_hwnd);

    m_hInstance = hInstance;

    m_justResized = true;
}

Window::~Window()
{
}

void Window::Update()
{
    MSG msg;
    ZeroMemory(&msg, sizeof(MSG));

    BOOL PeekMessageL(
        LPMSG lpMsg,
        HWND hWnd,
        UINT wMsgFilterMin,
        UINT wMsgFilterMax,
        UINT wRemoveMsg
    );

    if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (m_justResized)
        OnResized();
}

void Window::SetResolution(int width, int height)
{
    m_resWidth = width;
    m_resHeight = height;
    
    for (auto& callback : m_resolutionChangeListener)
    {
        callback(width, height);
    }
}

void Window::AddResizeListener(void(*callback)(const int&, const int&))
{
    m_resizeListeners.insert(callback);
}

void Window::RemoveResizeListener(void(*callback)(const int&, const int&))
{
    m_resizeListeners.erase(callback);
}

void Window::AddResolutionChangeListener(void(*callback)(const int&, const int&))
{
    m_resolutionChangeListener.insert(callback);
}

void Window::RemoveResolutionChangeListener(void(*callback)(const int&, const int&))
{
    m_resolutionChangeListener.erase(callback);
}
