#include "WindowTools.h"

namespace WindowTools
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

        if (g_ClientWidth != width || g_ClientHeight != height)
        {
            // Don't allow 0 size swap chain back buffers.
            g_ClientWidth = std::max(1u, width );
            g_ClientHeight = std::max( 1u, height);

            // Flush the GPU queue to make sure the swap chain's back buffers
            // are not being referenced by an in-flight command list.
            dx_ctx.Flush(dx_ctx.g_CommandQueue, dx_ctx.g_Fence, dx_ctx.g_FenceValue, dx_ctx.g_FenceEvent);
            for (int i = 0; i < dx_ctx.g_NumFrames; ++i)
            {
                // Any references to the back buffers must be released
                // before the swap chain can be resized.
                dx_ctx.g_BackBuffers[i].Reset();
                dx_ctx.g_FrameFenceValues[i] = dx_ctx.g_FrameFenceValues[dx_ctx.g_CurrentBackBufferIndex];
            }
            DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
            ThrowIfFailed(dx_ctx.g_SwapChain->GetDesc(&swapChainDesc));
            ThrowIfFailed(dx_ctx.g_SwapChain->ResizeBuffers(dx_ctx.g_NumFrames, g_ClientWidth, g_ClientHeight,
                                                     swapChainDesc.BufferDesc.Format, swapChainDesc.Flags));

            dx_ctx.g_CurrentBackBufferIndex = dx_ctx.g_SwapChain->GetCurrentBackBufferIndex();

            dx_ctx.UpdateRenderTargetViews(dx_ctx.g_Device, dx_ctx.g_SwapChain, dx_ctx.g_RTVDescriptorHeap);
        }

    }

    void SetFullscreen(bool fullscreen)
    {
        if (dx_ctx.g_Fullscreen != fullscreen)
        {
            dx_ctx.g_Fullscreen = fullscreen;

            if (dx_ctx.g_Fullscreen) // Switching to fullscreen.
            {
                // Store the current window dimensions so they can be restored
                // when switching out of fullscreen state.
                ::GetWindowRect(g_hWnd, &g_WindowRect);
                // Set the window style to a borderless window so the client area fills
                // the entire screen.
                UINT windowStyle = WS_OVERLAPPEDWINDOW & ~(WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);

                ::SetWindowLongW(g_hWnd, GWL_STYLE, windowStyle);
                // Query the name of the nearest display device for the window.
                // This is required to set the fullscreen dimensions of the window
                // when using a multi-monitor setup.
                HMONITOR hMonitor = ::MonitorFromWindow(g_hWnd, MONITOR_DEFAULTTONEAREST);
                MONITORINFOEX monitorInfo = {};
                monitorInfo.cbSize = sizeof(MONITORINFOEX);
                ::GetMonitorInfo(hMonitor, &monitorInfo);
                ::SetWindowPos(g_hWnd, HWND_TOP,
                               monitorInfo.rcMonitor.left,
                               monitorInfo.rcMonitor.top,
                               monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
                               monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
                               SWP_FRAMECHANGED | SWP_NOACTIVATE);

                ::ShowWindow(g_hWnd, SW_MAXIMIZE);
            }
            else
            {
                // Restore all the window decorators.
                ::SetWindowLong(g_hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);

                ::SetWindowPos(g_hWnd, HWND_NOTOPMOST,
                               g_WindowRect.left,
                               g_WindowRect.top,
                               g_WindowRect.right - g_WindowRect.left,
                               g_WindowRect.bottom - g_WindowRect.top,
                               SWP_FRAMECHANGED | SWP_NOACTIVATE);

                ::ShowWindow(g_hWnd, SW_NORMAL);
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
            OutputDebugString(buffer);

            frameCounter = 0;
            elapsedSeconds = 0.0;
        }
    }

    // Window callback function.
    LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        if ( dx_ctx.g_IsInitialized )
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
                            dx_ctx.g_VSync = !dx_ctx.g_VSync;
                            break;
                        case VK_ESCAPE:
                            ::PostQuitMessage(0);
                            break;
                        case VK_RETURN:
                            if ( alt )
                            {
                                case VK_F11:
                                    SetFullscreen(!dx_ctx.g_Fullscreen);
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
                    ::GetClientRect(g_hWnd, &clientRect);

                    int width = clientRect.right - clientRect.left;
                    int height = clientRect.bottom - clientRect.top;

                    Resize(width, height);
                }
                    break;
                case WM_DESTROY:
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

    void InitWindow(HINSTANCE hInstance)
    {
        // Windows 10 Creators update adds Per Monitor V2 DPI awareness context.
        // Using this awareness context allows the client area of the window
        // to achieve 100% scaling while still allowing non-client window content to
        // be rendered in a DPI sensitive fashion.
        SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

        const wchar_t* windowClassName = L"RayVox-Engine";

        RegisterWindowClass(hInstance, windowClassName);
        g_hWnd = createWindow(windowClassName, hInstance, L"Learning DirectX 12",
                              g_ClientWidth, g_ClientHeight);

        // Initialize the global window rect variable.
        ::GetWindowRect(g_hWnd, &g_WindowRect);

        dx_ctx.InitContext(g_hWnd, g_ClientWidth, g_ClientHeight);

        ::ShowWindow(g_hWnd, SW_SHOW);
    }
}
