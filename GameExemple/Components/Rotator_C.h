#pragma once

#include "Reflection/ComponentRegistry.h"

namespace batap
{
struct Rotator_C
{
    float speed_ = 1.f;
};

BATAP_COMPONENT(Rotator_C, "rotator");
}  // namespace batap
