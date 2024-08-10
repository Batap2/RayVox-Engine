#include "InputManager.h"
#include <Windowsx.h>

void InputManager::manageInput(UINT message, WPARAM wParam, LPARAM lParam)
{

    if(message == WM_KEYDOWN)
    {
        switch (wParam) {
            case 'D':
                ctx->camera.move(ctx->camera.getRightVec());
                ctx->computeAndUploadCameraBuffer();
                break;
            case 'Q':
                ctx->camera.move(XMVectorNegate(ctx->camera.getRightVec()));
                ctx->computeAndUploadCameraBuffer();
                break;
            case 'Z':
                ctx->camera.move(ctx->camera.getForwardVec());
                ctx->computeAndUploadCameraBuffer();
                break;
            case 'S':
                ctx->camera.move(XMVectorNegate(ctx->camera.getForwardVec()));
                ctx->computeAndUploadCameraBuffer();
                break;
            case 'A':
                ctx->camera.move({0,1,0});
                ctx->computeAndUploadCameraBuffer();
                break;
            case 'E':
                ctx->camera.move({0,-1,0});
                ctx->computeAndUploadCameraBuffer();
                break;
        }
    }

    if(message == WM_KEYUP)
    {

    }

}

void InputManager::ProcessRawInput(LPARAM lParam)
{
    auto hRawInput = (HRAWINPUT)lParam;
    RAWINPUT raw;
    UINT size;

    GetRawInputData(hRawInput, RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER));

    if (GetRawInputData(hRawInput, RID_INPUT, &raw, &size, sizeof(RAWINPUTHEADER)) == size)
    {
        if (raw.header.dwType == RIM_TYPEMOUSE)
        {
            // Traiter les données de la souris
            float mouseDeltaX = raw.data.mouse.lLastX * 0.001;
            float mouseDeltaY = raw.data.mouse.lLastY * 0.001;


            ctx->camera.rotate({0,1,0}, mouseDeltaX);
            ctx->camera.rotate(ctx->camera.getRightVec(), -mouseDeltaY);
            ctx->computeAndUploadCameraBuffer();
        }
    }
}