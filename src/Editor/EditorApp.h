#pragma once

// Editor as a library. An editor executable is three lines: include your
// GameComponents.h (static init registers the game's components into this
// binary), then define editorConfig(). The platform entry point
// (EntryPoint.cpp, added by batap_add_editor) calls runEditor(editorConfig()).

#include "Engine.h"

namespace batap
{
struct EditorConfig
{
    WindowDesc window_{.title = "Batap Engine", .fpsInTitle = true, .transparent = true};
};

// Defined by each editor executable.
EditorConfig editorConfig();

// Engine + World + App loop, with the top-level try/catch.
int runEditor(const EditorConfig& cfg = {});
}  // namespace batap
