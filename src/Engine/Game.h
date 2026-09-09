#pragma once

namespace batap
{
struct World;
struct Frame;

// The game as the engine drives it. The standalone main and the editor's Play
// button call the same two functions: init() when play begins (the scene is
// already loaded — spawn, grab references), update() once per frame.
// Scene loading itself stays outside: the standalone main loads the game's
// scene, the editor plays whatever scene is open.
struct Game
{
    virtual ~Game() = default;
    virtual void init(World&) {}
    virtual void update(World&, Frame&) {}
};
}  // namespace batap
