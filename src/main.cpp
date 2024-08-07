#define WIN32_LEAN_AND_MEAN

#include "App.h"
#include "DX12Context.h"

#include <iostream>
#include <string>

using namespace App;



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

    dx_cctx.flush();

    return 0;
}