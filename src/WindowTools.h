#pragma once

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <shellapi.h> // For CommandLineToArgvW
// Windows Runtime Library. Needed for Microsoft::WRL::ComPtr<> template class.
#include <wrl.h>
using namespace Microsoft::WRL;

#include <cstdint>
#include <algorithm>
#include <cassert>

#include "DX12Context.h"
#include "AssertUtils.h"

namespace WindowTools
{
    inline HWND g_hWnd;
    inline RECT g_WindowRect;
    inline uint32_t g_ClientWidth = 1080;
    inline uint32_t g_ClientHeight = 720;

    inline DX12Context dx_ctx;

    void RedirectIOToConsole();

    HWND createWindow(const wchar_t* windowClassName, HINSTANCE hInst,
                      const wchar_t* windowTitle, uint32_t width, uint32_t height);

    void Resize(uint32_t width, uint32_t height);

    void SetFullscreen(bool fullscreen);

    void Update();

    LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

    void RegisterWindowClass( HINSTANCE hInst, const wchar_t* windowClassName );

    void InitApp(HINSTANCE hInstance);
}