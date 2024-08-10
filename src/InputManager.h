#pragma once

#include "includeDX12.h"
#include "DX12ComputeContext.h"

struct InputManager
{
    DX12ComputeContext* ctx;

    int lastMousePos[2] = {-1,-1};

    void manageInput(UINT message, WPARAM wParam, LPARAM lParam);

    void ProcessRawInput(LPARAM hRawInput);
};