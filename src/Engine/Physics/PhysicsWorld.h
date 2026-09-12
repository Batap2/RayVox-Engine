#pragma once

#include <Jolt/Jolt.h>

#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/PhysicsSystem.h>

#include "Physics/PhysicsLayers.h"

namespace batap
{

struct JoltRuntime
{
    JoltRuntime();
    ~JoltRuntime();

    JoltRuntime(const JoltRuntime&) = delete;
    JoltRuntime& operator=(const JoltRuntime&) = delete;
};

struct PhysicsWorld
{
    PhysicsWorld();
    ~PhysicsWorld();

    PhysicsWorld(const PhysicsWorld&) = delete;
    PhysicsWorld& operator=(const PhysicsWorld&) = delete;

    void clear();
    void step(float dt);

    JPH::PhysicsSystem& system() { return system_; }
    JPH::BodyInterface& bodies() { return system_.GetBodyInterface(); }

   private:
    // Declared first: TempAllocatorImpl allocates through Jolt's allocator,
    // which only exists once JoltRuntime has registered it.
    JoltRuntime runtime_;

    JPH::TempAllocatorImpl tempAllocator_;
    JPH::JobSystemThreadPool jobs_;

    // Init() keeps references to these three, so they must outlive system_:
    // declared before it, hence destroyed after it.
    BroadPhaseLayerMap bpLayers_;
    ObjectVsBroadPhaseFilter objVsBp_;
    ObjectPairFilter objPair_;

    JPH::PhysicsSystem system_;
};

}  // namespace batap
