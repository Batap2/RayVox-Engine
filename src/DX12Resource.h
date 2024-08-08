#pragma once

#include "includeDX12.h"
#include "AssertUtils.h"

struct DX12Resource
{
    ComPtr<ID3D12Resource> resource;
    D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptorHandle = {};
    D3D12_GPU_DESCRIPTOR_HANDLE gpuDescriptorHandle = {};

    template <typename T>
    void createOrUpdateConstantBuffer(ID3D12Device* device,
                                                    ID3D12GraphicsCommandList* commandList,
                                                    const T& data,
                                                    ID3D12DescriptorHeap* descriptorHeap)
    {
        //TODO : check si sizeof(T) est un multiple de ... pour le padding
        const UINT64 bufferSize = (sizeof(T) + 255) & ~255; // Align buffer size to 256 bytes

        if (!resource)
        {
            // Create the constant buffer
            D3D12_HEAP_PROPERTIES heapProps = {};
            heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
            heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
            heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

            D3D12_RESOURCE_DESC bufferDesc = {};
            bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
            bufferDesc.Width = bufferSize;
            bufferDesc.Height = 1;
            bufferDesc.DepthOrArraySize = 1;
            bufferDesc.MipLevels = 1;
            bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
            bufferDesc.SampleDesc.Count = 1;
            bufferDesc.SampleDesc.Quality = 0;
            bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
            bufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

            ThrowIfFailed(device->CreateCommittedResource(
                    &heapProps,
                    D3D12_HEAP_FLAG_NONE,
                    &bufferDesc,
                    D3D12_RESOURCE_STATE_GENERIC_READ,
                    nullptr,
                    IID_PPV_ARGS(&resource)
            ));

            cpuDescriptorHandle = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
            D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
            cbvDesc.BufferLocation = resource->GetGPUVirtualAddress();
            cbvDesc.SizeInBytes = bufferSize;

            device->CreateConstantBufferView(&cbvDesc, cpuDescriptorHandle);
        }

        // Map and update the buffer
        void* pMappedData = nullptr;
        D3D12_RANGE readRange = { 0, 0 }; // We do not intend to read from this buffer

        ThrowIfFailed(resource->Map(0, &readRange, &pMappedData));

        std::memcpy(pMappedData, &data, sizeof(T));

        resource->Unmap(0, nullptr);
    }
};