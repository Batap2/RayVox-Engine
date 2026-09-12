#include "Physics/PhysicsLayers.h"

namespace batap
{

BroadPhaseLayerMap::~BroadPhaseLayerMap() = default;
ObjectVsBroadPhaseFilter::~ObjectVsBroadPhaseFilter() = default;
ObjectPairFilter::~ObjectPairFilter() = default;

JPH::uint BroadPhaseLayerMap::GetNumBroadPhaseLayers() const
{
    return broadPhaseLayers::Count;
}

JPH::BroadPhaseLayer BroadPhaseLayerMap::GetBroadPhaseLayer(JPH::ObjectLayer layer) const
{
    return layer == objectLayers::NonMoving ? broadPhaseLayers::NonMoving
                                            : broadPhaseLayers::Moving;
}

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
const char* BroadPhaseLayerMap::GetBroadPhaseLayerName(JPH::BroadPhaseLayer layer) const
{
    return layer == broadPhaseLayers::NonMoving ? "NonMoving" : "Moving";
}
#endif

bool ObjectVsBroadPhaseFilter::ShouldCollide(JPH::ObjectLayer layer,
                                             JPH::BroadPhaseLayer bpLayer) const
{
    return layer == objectLayers::Moving || bpLayer == broadPhaseLayers::Moving;
}

bool ObjectPairFilter::ShouldCollide(JPH::ObjectLayer a, JPH::ObjectLayer b) const
{
    return a == objectLayers::Moving || b == objectLayers::Moving;
}

}  // namespace batap
