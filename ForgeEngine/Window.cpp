#include "Window.h"
#include <exception>

LRESULT CALLBACK Window::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
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
        Window* window = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
        window->SetAsDead();
        return 0;
    }

    return DefWindowProc(hwnd,
        msg,
        wParam,
        lParam);
}

Window::Window(const HINSTANCE& hInstance, const int& ShowWnd, const int& width, const int& height, const bool& windowed)
{
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
}