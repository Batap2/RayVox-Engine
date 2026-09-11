#pragma once

#include <cstdint>

namespace batap
{

// A set of component types, one bit each, used only to route a change to the
// GPU pools that read the component. Bits are claimed by the pools at their
// construction (usedComponentMask) — a component no pool reads never gets
// one, so gameplay components cost nothing and the 64 cap only counts
// GPU-read types.
using ComponentMask = uint64_t;

inline constexpr uint32_t InvalidComponentBit = 0xFFFFFFFFu;

template <class T>
uint32_t& componentBitSlot()
{
    static uint32_t bit = InvalidComponentBit;
    return bit;
}

inline ComponentMask maskOfBit(uint32_t bit)
{
    return bit == InvalidComponentBit ? ComponentMask{0} : (ComponentMask{1} << bit);
}

// Empty for a type no pool reads — writing to a CPU-only component is not an
// error, it just marks nothing.
template <class T>
ComponentMask componentMask()
{
    return maskOfBit(componentBitSlot<T>());
}
}  // namespace batap
