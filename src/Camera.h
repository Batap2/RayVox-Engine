#pragma once

#include <DirectXMath.h>
using namespace DirectX;

struct CameraBuffer
{
    XMFLOAT3 pos;
    float Znear;
    XMFLOAT3 forward;
    float Zfar;
    XMFLOAT3 right;
    float fov;
};

struct Camera
{
    XMFLOAT3 pos{};
    XMFLOAT3 right{};
    XMFLOAT3 up{};
    XMFLOAT3 forward{};

    float fov, aspectRatio, Znear, Zfar, speed = 0.05f;

    Camera() = default;
    Camera(const XMFLOAT3& position, const XMFLOAT3& direction, const XMFLOAT3& upVec,
           float fov, float aspectRatio, float Znear, float Zfar)
            : pos(position), forward(direction), up(upVec), fov(fov), aspectRatio(aspectRatio), Znear(Znear), Zfar(Zfar)
    {
        XMVECTOR dir = XMLoadFloat3(&forward);
        XMVECTOR upVecV = XMLoadFloat3(&up);
        XMVECTOR rightV = XMVector3Cross(upVecV, dir);
        upVecV = XMVector3Cross(dir, rightV);

        dir = XMVector3Normalize(dir);
        rightV = XMVector3Normalize(rightV);
        upVecV = XMVector3Normalize(upVecV);

        XMStoreFloat3(&forward, dir);
        XMStoreFloat3(&right, rightV);
        XMStoreFloat3(&up, upVecV);
    }

    XMVECTOR getPosVec() const
    {
        return XMLoadFloat3(&pos);
    }

    XMVECTOR getForwardVec() const
    {
        return XMLoadFloat3(&forward);
    }

    XMVECTOR getUpVec() const
    {
        return XMLoadFloat3(&up);
    }

    XMVECTOR getRightVec() const
    {
        return XMLoadFloat3(&right);
    }

    void rotate(const XMVECTOR& axis, float angle)
    {
        XMMATRIX rotationMatrix = XMMatrixRotationAxis(axis, angle);

        XMVECTOR forwardVec = XMLoadFloat3(&forward);
        XMVECTOR rightVec = XMLoadFloat3(&right);
        XMVECTOR upVec = XMLoadFloat3(&up);

        forwardVec = XMVector3TransformNormal(forwardVec, rotationMatrix);
        rightVec = XMVector3TransformNormal(rightVec, rotationMatrix);
        upVec = XMVector3TransformNormal(upVec, rotationMatrix);

        XMStoreFloat3(&forward, forwardVec);
        XMStoreFloat3(&right, rightVec);
        XMStoreFloat3(&up, upVec);
    }

    void move(const XMVECTOR& direction)
    {
        XMVECTOR posVec = XMLoadFloat3(&pos);
        posVec = XMVectorAdd(posVec, XMVectorScale(direction, speed));
        XMStoreFloat3(&pos, posVec);
    }

    CameraBuffer getCameraBuffer()
    {
        return {pos, Znear, forward, Zfar, right, fov};
    }
};