#pragma once

#ifndef _INPUTCLASS_H_
#define _INPUTCLASS_H_

#include <DirectXMath.h>

#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")

#include <dinput.h>

using namespace DirectX;

class InputClass
{
public:
    static bool Initialize(HINSTANCE, HWND);

    static void UpdateInput();

    static bool GetKey(unsigned int);
    static bool GetKeyDown(unsigned int);
    static bool GetKeyUp(unsigned int);
    static XMFLOAT3 GetMouseDeltaPosition();
    static bool IsMouseButtonDown(unsigned int input);
    static XMFLOAT2 GetMouseCurrentPosition();

private:
    static BYTE m_keyboardState[256];
    static BYTE m_prevKeyboardState[256];

    static HWND m_hwnd;

    static IDirectInputDevice8* m_DIKeyboard;
    static IDirectInputDevice8* m_DIMouse;

    static DIMOUSESTATE m_lastMouseState;
    static LPDIRECTINPUT8 m_directInput;
};

#endif