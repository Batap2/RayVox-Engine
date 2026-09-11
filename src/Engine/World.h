#pragma once

#include <memory>
#include <string>
#include <string_view>
#include "Components/EntityHandle.h"
#include "Instance/InstanceManager.h"
#include "Renderer/SceneBinding.h"

#include <entt/entt.hpp>

namespace batap
{

struct Engine;
struct Systems;
struct EntityFactory;
struct AssetManager;
struct Spawnable;

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

    EntityHandle spawn(std::string_view spawnableId);
    EntityHandle spawn(const Spawnable& spawnable);
    void destroy(EntityHandle h);

    Systems& systems() { return *systems_; }
    GPUInstanceManager& instances() { return *instanceManager_; }
    EntityFactory& factory() { return *entityFactory_; }

    entt::registry registry_;

   private:
    std::unique_ptr<Systems> systems_;
    std::unique_ptr<GPUInstanceManager> instanceManager_;
    std::unique_ptr<EntityFactory> entityFactory_;

    Engine* ctx_ = nullptr;
};
}  // namespace batap
