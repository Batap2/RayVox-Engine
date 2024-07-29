#define WIN32_LEAN_AND_MEAN

#include "WindowTools.h"
#include "DX12Context.h"

#include <iostream>
#include <string>

using namespace WindowTools;



int CALLBACK wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow)
{

    RedirectIOToConsole();

    InitApp(hInstance);

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
    dx_ctx.m_DirectCommandQueue->Flush();
    dx_ctx.m_ComputeCommandQueue->Flush();
    dx_ctx.m_CopyCommandQueue->Flush();
    

    return 0;
}