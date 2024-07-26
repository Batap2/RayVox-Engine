#define WIN32_LEAN_AND_MEAN

#include "WindowTools.h"
#include "DX12Context.h"

#include <iostream>
#include <string>

using namespace WindowTools;


int CALLBACK wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow)
{

    InitWindow(hInstance);

    MSG msg = {};
    while (msg.message != WM_QUIT)
    {
        if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
    }

    // Make sure the command queue has finished all commands before closing.
    dx_ctx.Flush(dx_ctx.g_CommandQueue, dx_ctx.g_Fence, dx_ctx.g_FenceValue, dx_ctx.g_FenceEvent);

    ::CloseHandle(dx_ctx.g_FenceEvent);

    return 0;
}