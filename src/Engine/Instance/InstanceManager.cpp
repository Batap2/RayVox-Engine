#include "InstanceManager.h"
#include <cstdint>
#include <cstring>
#include "Components/EntityHandle.h"
#include "Renderer/Renderer.h"
#include "Renderer/ResourceManager.h"
#include "InstanceDeclaration.h"

namespace batap
{
GPUInstanceManager::GPUInstanceManager(Engine& ctx)
    : resourceManager_(*ctx.renderer_->resourceManager_) {};

// The registry usually outlives the manager, so the hooks must go before the
// pools they would call into.
GPUInstanceManager::~GPUInstanceManager()
{
    if (!registry_)
        return;

    pools_.forEach(
        [&](auto& pool)
        {
            using InstanceT = typename std::remove_reference_t<decltype(pool)>::InstanceType;
            using Marker = MarkerOf<InstanceT>;
            registry_->on_construct<Marker>().disconnect(this);
            registry_->on_destroy<Marker>().disconnect(this);
        });
}

void GPUInstanceManager::connectHooks(entt::registry& reg)
{
    registry_ = &reg;
    pools_.forEach(
        [&](auto& pool)
        {
            using InstanceT = typename std::remove_reference_t<decltype(pool)>::InstanceType;
            using Marker = MarkerOf<InstanceT>;
            reg.on_construct<Marker>()
                .template connect<&GPUInstanceManager::onMarkerCreated<InstanceT>>(*this);
            reg.on_destroy<Marker>()
                .template connect<&GPUInstanceManager::onMarkerDestroyed<InstanceT>>(*this);
        });
}

void GPUInstanceManager::uploadRemainingFrameDirty(Engine& ctx)
{
    auto frameIndex = ctx.getFrameindex();
    auto upload = [&](auto& frameInstancePool)
    {
        using PoolT = std::remove_reference_t<decltype(frameInstancePool)>;
        using InstanceT = typename PoolT::InstanceType;
        using GPUData = typename InstanceT::GPUData;

        auto& map = frameInstancePool.dirtyInstances_;
        for (auto it = map.begin(); it != map.end();)
        {
            const EntityHandle& entityHandle = it->first;
            FrameDirtyFlag& frameDirtyFlag = it->second;
            if (!frameDirtyFlag.dirty(frameIndex))
            {
                ++it;
                continue;
            }

            GPUInstanceID id = frameInstancePool.getGPUIndex(entityHandle);
            if (!id.valid())
            {
                it = map.erase(it);
                continue;
            }

            // Built on the stack so staging is only ever written linearly.
            GPUData data{};
            InstanceT::fill({ctx, *entityHandle.reg_, entityHandle.entity_}, data);

            auto span = resourceManager_.requestUpload(frameInstancePool.instancePoolHandle_,
                                                       sizeof(GPUData), id * sizeof(GPUData));
            std::memcpy(span.data(), &data, sizeof(GPUData));

            frameDirtyFlag.clear(frameIndex);

            if (frameDirtyFlag.none())
            {
                it = map.erase(it);
                continue;
            }
            ++it;
        }
    };

    pools_.forEach(upload);
}

void GPUInstanceManager::markDirty(const EntityHandle& handle, ComponentMask changed)
{
    // Bailing out here rather than at each call site lets generic code — the
    // field loops, Scene::write<T> — mark anything unconditionally: a CPU-only
    // component has an empty mask and reaches no pool.
    if (changed == 0)
        return;

    forEachPool(
        [&](auto& pool)
        {
            if ((changed & pool.usedComponents_) == 0 || !pool.contains(handle))
                return;

            pool.dirtyInstances_[handle].setAll();
        });
}
}  // namespace batap
