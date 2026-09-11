#pragma once

#include "Components/Rotator_C.h"
#include "Components/Transform_C.h"
#include "batap.h"

namespace batap
{
struct MyGame : Game
{
    void update(World& world, float dt) override
    {
        auto& reg = world.registry_;
        for (auto [e, rot] : reg.view<Rotator_C>().each())
        {
            EntityHandle h{&reg, e};
            auto* t = h.try_get<Transform_C>();
            if (!t)
                continue;
            const quatf q =
                (t->rot() * angleaxisf(rot.speed_ * dt, v3f::UnitZ())).normalized();
            h.setLocalRotation(q);
        }
        world.spawn("mesh");
    }
};
}  // namespace batap
