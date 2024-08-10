#pragma once

#include "includeDX12.h"
#include "DX12ComputeContext.h"

struct InputManager
{
    DX12ComputeContext* ctx;

    bool holdedKey[9] = {false};
    enum KeyIndex
    {
        Forward,
        Backward,
        Left,
        Right,
        Up,
        Down,
        SHIFT,
        CTRL,
        SPACE
    };

    void manageInput(UINT message, WPARAM wParam, LPARAM lParam);

    void ProcessRawInput(LPARAM hRawInput);

    void processTickInput();
};