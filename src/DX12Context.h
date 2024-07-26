#pragma once

#include <wrl.h>
using namespace Microsoft::WRL;

// undef minwindef.h macro
#if defined(min)
#undef min
#endif

#if defined(max)
#undef max
#endif

#include <DirectX-Headers/include/directx/d3dx12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#include <chrono>
#include <cstdint>
#include <iostream>

#include "AssertUtils.h"

struct DX12Context{
    // The number of swap chain back buffers.
    static const uint8_t g_NumFrames = 3;
    // Use WARP adapter
    bool g_UseWarp = false;

    // Set to true once the DX12 objects have been initialized.
    bool g_IsInitialized = false;

    bool g_VSync = true;
    bool g_TearingSupported = false;
    bool g_Fullscreen = false;

    // DirectX 12 Objects
    ComPtr<ID3D12Device2> g_Device;
    ComPtr<ID3D12CommandQueue> g_CommandQueue;
    ComPtr<IDXGISwapChain4> g_SwapChain;
    ComPtr<ID3D12Resource> g_BackBuffers[g_NumFrames];
    ComPtr<ID3D12GraphicsCommandList> g_CommandList;
    ComPtr<ID3D12CommandAllocator> g_CommandAllocators[g_NumFrames];
    ComPtr<ID3D12DescriptorHeap> g_RTVDescriptorHeap; // array of rtv for the backbuffers of the swapchain
    UINT g_RTVDescriptorSize;
    UINT g_CurrentBackBufferIndex;

    // Synchronization objects
    ComPtr<ID3D12Fence> g_Fence;
    uint64_t g_FenceValue = 0;
    uint64_t g_FrameFenceValues[g_NumFrames] = {};
    HANDLE g_FenceEvent;

    void EnableDebugLayer();

    ComPtr<IDXGIAdapter4> GetAdapter(bool useWarp);

    ComPtr<ID3D12Device3> CreateDevice(ComPtr<IDXGIAdapter4>& adapter);

    ComPtr<ID3D12CommandQueue> CreateCommandQueue(ComPtr<ID3D12Device2> &device, D3D12_COMMAND_LIST_TYPE type);

    bool CheckTearingSupport();

    ComPtr<IDXGISwapChain4> CreateSwapChain(HWND hWnd,
                                            ComPtr<ID3D12CommandQueue> &commandQueue,
                                            uint32_t width, uint32_t height, uint32_t bufferCount);

    ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(ComPtr<ID3D12Device2> &device,
                                                      D3D12_DESCRIPTOR_HEAP_TYPE type,
                                                      uint32_t numDescriptors);

    void UpdateRenderTargetViews(ComPtr<ID3D12Device2> &device,
                                 ComPtr<IDXGISwapChain4> &swapChain,
                                 ComPtr<ID3D12DescriptorHeap> &descriptorHeap);

    ComPtr<ID3D12CommandAllocator> CreateCommandAllocator(ComPtr<ID3D12Device2> &device,
                                                          D3D12_COMMAND_LIST_TYPE type);

    ComPtr<ID3D12GraphicsCommandList> CreateCommandList(ComPtr<ID3D12Device2> &device,
                                                        ComPtr<ID3D12CommandAllocator> &commandAllocator,
                                                        D3D12_COMMAND_LIST_TYPE type);

    ComPtr<ID3D12Fence> CreateFence(ComPtr<ID3D12Device2> &device);

    HANDLE CreateEventHandle();

    uint64_t Signal(ComPtr<ID3D12CommandQueue> &commandQueue,
                    ComPtr<ID3D12Fence> &fence,
                    uint64_t& fenceValue);

    void WaitForFenceValue(ComPtr<ID3D12Fence> &fence, uint64_t fenceValue, HANDLE fenceEvent,
                           std::chrono::milliseconds duration = std::chrono::milliseconds::max());

    void Render();

    void Flush(ComPtr<ID3D12CommandQueue> &commandQueue, ComPtr<ID3D12Fence> &fence,
                            uint64_t& fenceValue, HANDLE fenceEvent);

    void InitContext(HWND hWnd, uint32_t clientWidth, uint32_t clientHeight);
};
