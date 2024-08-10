#pragma once

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <shellapi.h> // For CommandLineToArgvW
// Windows Runtime Library. Needed for Microsoft::WRL::ComPtr<> template class.
#include <wrl.h>
using namespace Microsoft::WRL;

#include <cstdint>

#include "DX12ComputeContext.h"
#include "InputManager.h"

namespace App
{
    inline HWND hWnd;
    inline RECT windowRect;
    inline uint32_t clientWidth = 800;
    inline uint32_t clientHeight = 800;

    inline DX12ComputeContext dx_cctx;
    inline InputManager inputManager;

    inline bool isWindowFocused = false;

    void RedirectIOToConsole();

    HWND createWindow(const wchar_t* windowClassName, HINSTANCE hInst,
                      const wchar_t* windowTitle, uint32_t width, uint32_t height);

    void Resize(uint32_t width, uint32_t height);

    void SetFullscreen(bool fullscreen);

    void Update();

    LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

    void RegisterWindowClass( HINSTANCE hInst, const wchar_t* windowClassName );

    void RegisterRawInputDevices(HWND hwnd);

    void InitApp(HINSTANCE hInstance);
}