#pragma once

#include <d3d12.h>
#include <DirectXMath.h>
#include <wrl.h>
using namespace Microsoft::WRL;

struct VertexData
{
    DirectX::XMFLOAT3 pos;
};

struct PlaneRenderTarget
{
    ComPtr<ID3D12Resource> vertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView;

    ComPtr<ID3D12Resource> indexBuffer;
    D3D12_INDEX_BUFFER_VIEW indexBufferView;

    VertexData vertexDataArray[4]{
            {{0,0,0}},
            {{1,0,0}},
            {{0,1,0}},
            {{1,1,0}}
    };

    WORD indices[6]{
        0,1,2,1,3,2
    };
};
