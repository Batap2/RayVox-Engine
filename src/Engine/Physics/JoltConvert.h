#pragma once

#include <Jolt/Jolt.h>

#include <Jolt/Math/Quat.h>
#include <Jolt/Math/Vec3.h>

#include "EigenTypes.h"

namespace batap
{

inline JPH::Vec3 toJolt(const v3f& v)
{
    return JPH::Vec3(v.x(), v.y(), v.z());
}

inline JPH::Quat toJolt(const quatf& q)
{
    return JPH::Quat(q.x(), q.y(), q.z(), q.w());
}

inline v3f toEigen(JPH::Vec3Arg v)
{
    return v3f(v.GetX(), v.GetY(), v.GetZ());
}

inline quatf toEigen(JPH::QuatArg q)
{
    return quatf(q.GetW(), q.GetX(), q.GetY(), q.GetZ());
}

}  // namespace batap
