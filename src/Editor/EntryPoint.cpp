// Platform entry point for editor executables. Not part of Batap_EditorLib:
// batap_add_editor compiles it into each exe so the subsystem (wWinMain vs
// main) is decided once, here.

#include "EditorApp.h"

#if defined(_WIN32)

#include <windows.h>

int CALLBACK wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
    return batap::runEditor(batap::editorConfig());
}

#else

int main()
{
    return batap::runEditor(batap::editorConfig());
}

#endif
