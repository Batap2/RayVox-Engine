#pragma once

// Editor as a library. An editor executable is three lines: include your
// GameComponents.h (static init registers the game's components into this
// binary), then define editorConfig(). The platform entry point
// (EntryPoint.cpp, added by batap_add_editor) calls runEditor(editorConfig()).

#include "Engine.h"
#include "Game.h"

#include <memory>

namespace batap
{
struct EditorConfig
{
    WindowDesc window_{.title = "Batap Engine", .fpsInTitle = true, .transparent = true};

    // The game the Play button runs. Null = bare editor: Play still
    // snapshots/restores the scene, but no game logic ticks.
    std::unique_ptr<Game> (*makeGame_)() = nullptr;

    // Name of the game executable next to the editor's (no extension). Set,
    // it adds a "Run" button: current scene saved to a temp file, game spawned
    // in its own process — a crash there never takes the editor down.
    const char* gameExeName_ = nullptr;
};

// For EditorConfig::makeGame_: `.makeGame_ = makeGame<MyGame>`.
template <class G>
std::unique_ptr<Game> makeGame()
{
    return std::make_unique<G>();
}

// Defined by each editor executable.
EditorConfig editorConfig();

// Engine + World + App loop, with the top-level try/catch.
int runEditor(const EditorConfig& cfg = {});
}  // namespace batap
