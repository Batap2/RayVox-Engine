#pragma once

#include <memory>
#include <string>
#include "Renderer/SceneBinding.h"
#include "Scene.h"

namespace batap
{

struct Engine;
struct Systems;
struct GPUInstanceManager;
struct EntityFactory;
struct AssetManager;

struct World
{
    World(Engine& engine);
    ~World();

    void update();
    SceneRenderArgs renderArgs();
    bool loadScene(const std::string& path);

    // Replaces the registry itself: its per-type storages hold code pointers
    // into a loaded game DLL and must not outlive it (hot reload).
    void resetScene();

    std::unique_ptr<Scene> scene_;
    std::unique_ptr<Systems> systems_;
    std::unique_ptr<GPUInstanceManager> instanceManager_;
    std::unique_ptr<EntityFactory> entityFactory_;

   private:
    Engine* ctx_ = nullptr;
};
}  // namespace batap
