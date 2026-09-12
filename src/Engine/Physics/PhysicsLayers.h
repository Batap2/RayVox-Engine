#pragma once

#include <Jolt/Jolt.h>

#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>

namespace batap
{

namespace objectLayers
{
constexpr JPH::ObjectLayer NonMoving = 0;
constexpr JPH::ObjectLayer Moving = 1;
constexpr JPH::ObjectLayer Count = 2;
}  // namespace objectLayers

namespace broadPhaseLayers
{
constexpr JPH::BroadPhaseLayer NonMoving{0};
constexpr JPH::BroadPhaseLayer Moving{1};
constexpr JPH::uint Count = 2;
}  // namespace broadPhaseLayers

struct BroadPhaseLayerMap final : JPH::BroadPhaseLayerInterface
{
    ~BroadPhaseLayerMap() override;

    JPH::uint GetNumBroadPhaseLayers() const override;
    JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer layer) const override;
#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
    const char* GetBroadPhaseLayerName(JPH::BroadPhaseLayer layer) const override;
#endif
};

struct ObjectVsBroadPhaseFilter final : JPH::ObjectVsBroadPhaseLayerFilter
{
    ~ObjectVsBroadPhaseFilter() override;

    bool ShouldCollide(JPH::ObjectLayer layer, JPH::BroadPhaseLayer bpLayer) const override;
};

struct ObjectPairFilter final : JPH::ObjectLayerPairFilter
{
    ~ObjectPairFilter() override;

    bool ShouldCollide(JPH::ObjectLayer a, JPH::ObjectLayer b) const override;
};

}  // namespace batap
