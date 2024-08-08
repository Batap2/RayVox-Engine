#pragma once

#include <DirectXMath.h>
using namespace DirectX;

struct CameraBuffer
{
    XMMATRIX viewProjMatrix;
    XMFLOAT3 pos;
    float padding;
};

struct Camera
{
    XMMATRIX transform{};
    XMMATRIX viewProjMatrix{};

    float fov, aspectRatio, nearPlane, farPlane;

    Camera() = default;
    Camera(const XMFLOAT3& position, const XMFLOAT3& direction, const XMFLOAT3& up,
           float fov, float aspectRatio, float nearPlane, float farPlane)
           : fov(fov), aspectRatio(aspectRatio), nearPlane(nearPlane), farPlane(farPlane)
    {
        XMVECTOR pos = XMLoadFloat3(&position);
        XMVECTOR dir = XMLoadFloat3(&direction);
        XMVECTOR upVec = XMLoadFloat3(&up);

        transform = XMMatrixLookAtLH(pos, dir, upVec);

        computeViewProj();
    }

    void computeViewProj()
    {
        XMMATRIX viewMatrix = XMMatrixInverse(nullptr, transform);
        XMMATRIX projMatrix = XMMatrixPerspectiveFovLH(fov, aspectRatio, nearPlane, farPlane);
        viewProjMatrix = XMMatrixMultiply(viewMatrix, projMatrix);
    }

    XMFLOAT3 getPos() const
    {
        XMFLOAT3 pos;
        XMStoreFloat3(&pos, transform.r[3]);
        return pos;
    }

    XMFLOAT3 getForward() const
    {
        XMFLOAT3 forward;
        XMStoreFloat3(&forward, XMVector3Normalize(transform.r[2]));
        return forward;
    }

    XMFLOAT3 getUp() const
    {
        XMFLOAT3 up;
        XMStoreFloat3(&up, XMVector3Normalize(transform.r[1]));
        return up;
    }

    XMFLOAT3 getRight() const
    {
        XMFLOAT3 right;
        XMStoreFloat3(&right, XMVector3Normalize(transform.r[0]));
        return right;
    }

    void rotate(const XMFLOAT3& axis, float angle)
    {
        XMVECTOR axisVec = XMLoadFloat3(&axis);
        XMMATRIX rotationMatrix = XMMatrixRotationAxis(axisVec, angle);
        transform = XMMatrixMultiply(rotationMatrix, transform);
    }

    void move(const XMFLOAT3& direction)
    {
        XMVECTOR directionVec = XMLoadFloat3(&direction);
        XMMATRIX translationMatrix = XMMatrixTranslationFromVector(directionVec);
        transform = XMMatrixMultiply(transform, translationMatrix);
    }
};