#pragma once

#include "Components/Rotator_C.h"
#include "Components/Transform_C.h"
#include "Engine.h"
#include "Game.h"
#include "Systems/Systems.h"
#include "Systems/Transform_S.h"
#include "World.h"

namespace batap
{
struct MyGame : Game
{
    void update(World& world, Frame& frame) override
    {
        auto& reg = world.scene_->registry_;
        for (auto [e, rot] : reg.view<Rotator_C>().each())
        {
            EntityHandle h{&reg, e};
            auto* t = h.try_get<Transform_C>();
            if (!t)
                continue;
            const quatf q =
                (t->rot() * angleaxisf(rot.speed_ * frame.dt(), v3f::UnitY())).normalized();
            world.systems_->transforms_->setLocalRotation(h, q);
        }
    }
};
}  // namespace batap
