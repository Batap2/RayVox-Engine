#include "App.h"

#include <chrono>
#include <iostream>
#include <algorithm>
#include <cassert>

#include "AssertUtils.h"

namespace App
{
    HWND createWindow(const wchar_t* windowClassName, HINSTANCE hInst,
                      const wchar_t* windowTitle, uint32_t width, uint32_t height)
    {
        int screenWidth = ::GetSystemMetrics(SM_CXSCREEN);
        int screenHeight = ::GetSystemMetrics(SM_CYSCREEN);

        RECT windowRect = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };
        ::AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

        int windowWidth = windowRect.right - windowRect.left;
        int windowHeight = windowRect.bottom - windowRect.top;

        // Center the window within the screen. Clamp to 0, 0 for the top-left corner.
        int windowX = std::max<int>(0, (screenWidth - windowWidth) / 2);
        int windowY = std::max<int>(0, (screenHeight - windowHeight) / 2);
        HWND hWnd = ::CreateWindowExW(
                NULL,
                windowClassName,
                windowTitle,
                WS_OVERLAPPEDWINDOW,
                windowX,
                windowY,
                windowWidth,
                windowHeight,
                NULL,
                NULL,
                hInst,
                nullptr
        );

        assert(hWnd && "Failed to create window");

        return hWnd;
    }

    void Resize(uint32_t width, uint32_t height)
    {

        if (clientWidth != width || clientHeight != height)
        {
            // Don't allow 0 size swap chain back buffers.
            clientWidth = std::max(1u, width );
            clientHeight = std::max(1u, height);

            // Flush the GPU queue to make sure the swap chain's back buffers
            // are not being referenced by an in-flight command list.

            dx_ctx.directCommandQueue->Flush();
            dx_ctx.computeCommandQueue->Flush();
            dx_ctx.copyCommandQueue->Flush();

            for (int i = 0; i < dx_ctx.bufferCount; ++i)
            {
                // Any references to the back buffers must be released
                // before the swap chain can be resized.
                dx_ctx.backBuffers[i].Reset();
            }
            DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
            ThrowIfFailed(dx_ctx.swapChain->GetDesc(&swapChainDesc));
            ThrowIfFailed(dx_ctx.swapChain->ResizeBuffers(dx_ctx.bufferCount, clientWidth, clientHeight,
                                                          swapChainDesc.BufferDesc.Format, swapChainDesc.Flags));

            dx_ctx.currentBackBufferIndex = dx_ctx.swapChain->GetCurrentBackBufferIndex();

            dx_ctx.UpdateRenderTargetViews(dx_ctx.device, dx_ctx.swapChain, dx_ctx.RTVDescriptorHeap);
        }

    }

    void SetFullscreen(bool fullscreen)
    {
        if (dx_ctx.fullscreen != fullscreen)
        {
            dx_ctx.fullscreen = fullscreen;

            if (dx_ctx.fullscreen) // Switching to fullscreen.
            {
                // Store the current window dimensions so they can be restored
                // when switching out of fullscreen state.
                ::GetWindowRect(hWnd, &windowRect);
                // Set the window style to a borderless window so the client area fills
                // the entire screen.
                UINT windowStyle = WS_OVERLAPPEDWINDOW & ~(WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);

                ::SetWindowLongW(hWnd, GWL_STYLE, windowStyle);
                // Query the name of the nearest display device for the window.
                // This is required to set the fullscreen dimensions of the window
                // when using a multi-monitor setup.
                HMONITOR hMonitor = ::MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
                MONITORINFOEX monitorInfo = {};
                monitorInfo.cbSize = sizeof(MONITORINFOEX);
                ::GetMonitorInfo(hMonitor, &monitorInfo);
                ::SetWindowPos(hWnd, HWND_TOP,
                               monitorInfo.rcMonitor.left,
                               monitorInfo.rcMonitor.top,
                               monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
                               monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
                               SWP_FRAMECHANGED | SWP_NOACTIVATE);

                ::ShowWindow(hWnd, SW_MAXIMIZE);
            }
            else
            {
                // Restore all the window decorators.
                ::SetWindowLong(hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);

                ::SetWindowPos(hWnd, HWND_NOTOPMOST,
                               windowRect.left,
                               windowRect.top,
                               windowRect.right - windowRect.left,
                               windowRect.bottom - windowRect.top,
                               SWP_FRAMECHANGED | SWP_NOACTIVATE);

                ::ShowWindow(hWnd, SW_NORMAL);
            }
        }
    }

    void Update()
    {
        static uint64_t frameCounter = 0;
        static double elapsedSeconds = 0.0;
        static std::chrono::high_resolution_clock clock;
        static auto t0 = clock.now();

        frameCounter++;
        auto t1 = clock.now();
        auto deltaTime = t1 - t0;
        t0 = t1;
        elapsedSeconds += deltaTime.count() * 1e-9;
        if (elapsedSeconds > 1.0)
        {
            char buffer[500];
            auto fps = frameCounter / elapsedSeconds;
            sprintf_s(buffer, 500, "FPS: %f\n", fps);
            std::cout << buffer;

            frameCounter = 0;
            elapsedSeconds = 0.0;
        }
    }

    // Window callback function.
    LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        if ( dx_ctx.isInitialized )
        {
            switch (message)
            {
                case WM_PAINT:
                    Update();
                    dx_ctx.Render();
                    break;
                case WM_SYSKEYDOWN:
                case WM_KEYDOWN:
                {
                    bool alt = (::GetAsyncKeyState(VK_MENU) & 0x8000) != 0;

                    switch (wParam)
                    {
                        case 'V':
                            dx_ctx.useVSync = !dx_ctx.useVSync;
                            break;
                        case VK_ESCAPE:
                            ::PostQuitMessage(0);
                            break;
                        case VK_RETURN:
                            if ( alt )
                            {
                                case VK_F11:
                                    SetFullscreen(!dx_ctx.fullscreen);
                            }
                            break;
                    }
                }
                    break;
                    // The default window procedure will play a system notification sound
                    // when pressing the Alt+Enter keyboard combination if this message is
                    // not handled.
                case WM_SYSCHAR:
                    break;
                case WM_SIZE:
                {
                    RECT clientRect = {};
                    ::GetClientRect(hWnd, &clientRect);

                    int width = clientRect.right - clientRect.left;
                    int height = clientRect.bottom - clientRect.top;

                    Resize(width, height);
                }
                    break;
                case WM_DESTROY:
                case WM_CLOSE:
                    ::PostQuitMessage(0);
                    break;
                default:
                    return ::DefWindowProcW(hwnd, message, wParam, lParam);
            }
        }
        else
        {
            return ::DefWindowProcW(hwnd, message, wParam, lParam);
        }

        return 0;
    }

    void RegisterWindowClass( HINSTANCE hInst, const wchar_t* windowClassName )
    {
        // Register a window class for creating our render window with.
        WNDCLASSEXW windowClass = {};

        windowClass.cbSize = sizeof(WNDCLASSEX);
        windowClass.style = CS_HREDRAW | CS_VREDRAW;
        windowClass.lpfnWndProc = &WndProc;
        windowClass.cbClsExtra = 0;
        windowClass.cbWndExtra = 0;
        windowClass.hInstance = hInst;
        windowClass.hIcon = ::LoadIcon(hInst, NULL);
        windowClass.hCursor = ::LoadCursor(NULL, IDC_ARROW);
        windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        windowClass.lpszMenuName = NULL;
        windowClass.lpszClassName = windowClassName;
        windowClass.hIconSm = ::LoadIcon(hInst, NULL);

        static ATOM atom = ::RegisterClassExW(&windowClass);
        assert(atom > 0);
    }

    void RedirectIOToConsole()
    {
        // Allouer une console pour cette application.
        AllocConsole();

        // Rediriger la sortie standard vers la console.
        FILE* fp;
        freopen_s(&fp, "CONOUT$", "w", stdout);
        freopen_s(&fp, "CONOUT$", "w", stderr);

        // Optionnel: rediriger l'entrée standard de la console.
        freopen_s(&fp, "CONIN$", "r", stdin);

        // Synchroniser les flux de C++ avec ceux de C.
        std::ios::sync_with_stdio();
    }

    void InitApp(HINSTANCE hInstance)
    {

#if defined(_DEBUG)
        RedirectIOToConsole();
#endif

        // Windows 10 Creators update adds Per Monitor V2 DPI awareness context.
        // Using this awareness context allows the client area of the window
        // to achieve 100% scaling while still allowing non-client window content to
        // be rendered in a DPI sensitive fashion.
        SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

        const wchar_t* windowClassName = L"RayVox-Engine";

        RegisterWindowClass(hInstance, windowClassName);
        hWnd = createWindow(windowClassName, hInstance, L"RayVox-Engine",
                            clientWidth, clientHeight);

        // Initialize the global window rect variable.
        ::GetWindowRect(hWnd, &windowRect);

        dx_ctx.InitContext(hWnd, clientWidth, clientHeight);

        dx_ctx.InitShaders();

        ::ShowWindow(hWnd, SW_SHOW);
    }
}
