#include "InputClass.h"
#include <stdio.h>
#include <iostream>

BYTE InputClass::m_keyboardState[256];
BYTE InputClass::m_prevKeyboardState[256];

HWND InputClass::m_hwnd;

IDirectInputDevice8* InputClass::m_DIKeyboard;
IDirectInputDevice8* InputClass::m_DIMouse;

DIMOUSESTATE InputClass::m_lastMouseState;
LPDIRECTINPUT8 InputClass::m_directInput;


bool InputClass::Initialize(HINSTANCE hInstance, HWND hwnd)
{
    DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);

    HRESULT result;

    result = m_directInput->CreateDevice(GUID_SysKeyboard, &m_DIKeyboard, NULL);

    result = m_directInput->CreateDevice(GUID_SysMouse, &m_DIMouse, NULL);

    result = m_DIKeyboard->SetDataFormat(&c_dfDIKeyboard);
    result = m_DIKeyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

    result = m_DIMouse->SetDataFormat(&c_dfDIMouse);
    result = m_DIMouse->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);

    m_hwnd = hwnd;

    return !FAILED(result);
}

void InputClass::UpdateInput()
{
    m_DIKeyboard->Acquire();
    m_DIMouse->Acquire();

    std::copy(std::begin(m_keyboardState), std::end(m_keyboardState), std::begin(m_prevKeyboardState));
    m_DIKeyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
    m_DIMouse->GetDeviceState(sizeof(m_lastMouseState), (LPVOID)&m_lastMouseState);
}

bool InputClass::GetKey(unsigned int input)
{
    return (m_keyboardState[input] & 0x80) != 0;
}

bool InputClass::GetKeyDown(unsigned int input)
{
    return GetKey(input) && ((m_prevKeyboardState[input] & 0x80) == 0);
}

bool InputClass::GetKeyUp(unsigned int input)
{
    return !GetKey(input) && ((m_prevKeyboardState[input] & 0x80) != 0);
}

XMFLOAT3 InputClass::GetMouseDeltaPosition()
{
    return XMFLOAT3((float)m_lastMouseState.lX, (float)m_lastMouseState.lY, (float)m_lastMouseState.lZ);
}

bool InputClass::IsMouseButtonDown(unsigned int input)
{
    return m_lastMouseState.rgbButtons[input] != 0;
}

XMFLOAT2 InputClass::GetMouseCurrentPosition()
{
    POINT tmp;
    GetCursorPos(&tmp);
    ScreenToClient(m_hwnd, &tmp);
    return XMFLOAT2((float)tmp.x, (float)tmp.y);
}