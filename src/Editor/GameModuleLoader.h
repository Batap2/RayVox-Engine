#pragma once

#include "GameModule.h"

#include <memory>
#include <string>

namespace batap
{
struct GameModuleLoader
{
    ~GameModuleLoader();

    bool load(const std::string& dllPath);
    std::unique_ptr<Game> makeGame() const;
    bool loaded() const { return lib_ != nullptr; }

    GameModuleAPI api_{};
    void* lib_ = nullptr;
    std::string sourcePath_;
    uint32_t generation_ = 0;
};
}  // namespace batap
