#pragma once

#include <string>
#include <vector>

namespace batap
{
// Raw JSON of components whose type is not in the ComponentRegistry, kept
// verbatim from load so a save does not silently drop them.
struct UnknownComponents_C
{
    std::vector<std::string> blobs_;
};
}  // namespace batap
