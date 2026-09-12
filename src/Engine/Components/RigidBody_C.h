#pragma once

#include "EigenTypes.h"
#include "Reflection/ComponentRegistry.h"

#include <cstdint>

namespace batap
{

inline constexpr uint32_t kInvalidBodyId = 0xffffffffu;

struct RigidBody_C
{
    enum class Motion : uint32_t
    {
        Static = 0,
        Kinematic = 1,
        Dynamic = 2
    };

    enum class Shape : uint32_t
    {
        Box = 0,
        Sphere = 1,
        Capsule = 2
    };

    bool active_ = true;
    Motion motion_ = Motion::Dynamic;

    Shape shape_ = Shape::Box;
    // Only the dimensions its shape_ uses are read: halfExtents for a box,
    // radius for a sphere, radius + halfHeight for a capsule.
    v3f halfExtents_ = {0.5f, 0.5f, 0.5f};
    float radius_ = 0.5f;
    float halfHeight_ = 0.5f;

    float mass_ = 1.f;
    float friction_ = 0.2f;
    float restitution_ = 0.f;
    float linearDamping_ = 0.05f;
    float angularDamping_ = 0.05f;
    float gravityFactor_ = 1.f;

    uint32_t bodyId_ = kInvalidBodyId;
    v3f shapeScale_ = {1.f, 1.f, 1.f};
};

static_assert(refl::fieldName<RigidBody_C, 0>() == "active");
static_assert(refl::fieldName<RigidBody_C, 1>() == "motion");
static_assert(refl::fieldName<RigidBody_C, 2>() == "shape");
static_assert(refl::fieldName<RigidBody_C, 3>() == "halfExtents");
static_assert(refl::fieldName<RigidBody_C, 4>() == "radius");
static_assert(refl::fieldName<RigidBody_C, 5>() == "halfHeight");
static_assert(refl::fieldName<RigidBody_C, 6>() == "mass");
static_assert(refl::fieldName<RigidBody_C, 7>() == "friction");
static_assert(refl::fieldName<RigidBody_C, 8>() == "restitution");
static_assert(refl::fieldName<RigidBody_C, 9>() == "linearDamping");
static_assert(refl::fieldName<RigidBody_C, 10>() == "angularDamping");
static_assert(refl::fieldName<RigidBody_C, 11>() == "gravityFactor");

BATAP_COMPONENT(RigidBody_C, "rigidBody", fieldSkip<&RigidBody_C::bodyId_>(),
                fieldSkip<&RigidBody_C::shapeScale_>(),
                fieldMeta<&RigidBody_C::halfExtents_>({.speed = 0.01f, .min = 0.001f}),
                fieldMeta<&RigidBody_C::radius_>({.speed = 0.01f, .min = 0.001f, .max = 1000.f}),
                fieldMeta<&RigidBody_C::halfHeight_>({.speed = 0.01f, .min = 0.001f, .max = 1000.f}),
                fieldMeta<&RigidBody_C::mass_>({.speed = 0.05f, .min = 0.001f, .max = 10000.f}),
                fieldMeta<&RigidBody_C::friction_>({.speed = 0.01f, .min = 0.f, .max = 1.f}),
                fieldMeta<&RigidBody_C::restitution_>({.speed = 0.01f, .min = 0.f, .max = 1.f}),
                fieldMeta<&RigidBody_C::linearDamping_>({.speed = 0.01f, .min = 0.f, .max = 1.f}),
                fieldMeta<&RigidBody_C::angularDamping_>({.speed = 0.01f, .min = 0.f, .max = 1.f}),
                fieldMeta<&RigidBody_C::gravityFactor_>({.speed = 0.01f, .min = 0.f, .max = 10.f}));

}  // namespace batap
