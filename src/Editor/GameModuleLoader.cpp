#include "GameModuleLoader.h"

#include "Reflection/ComponentRegistry.h"
#include "UI/FieldUI.h"

#include <filesystem>
#include <iostream>

#if defined(_WIN32)
#include <windows.h>
#else
#include <dlfcn.h>
#endif

namespace batap
{

bool GameModuleLoader::load(const std::string& dllPath)
{
    namespace fs = std::filesystem;
    sourcePath_ = dllPath;

#if defined(_WIN32)
    // Windows locks loaded modules: load a copy so the linker can keep
    // overwriting the real one.
    fs::path loadedPath = fs::path(dllPath);
    loadedPath.replace_filename(loadedPath.stem().string() + "_loaded_" +
                                std::to_string(generation_++) + ".dll");
    std::error_code ec;
    fs::copy_file(dllPath, loadedPath, fs::copy_options::overwrite_existing, ec);
    if (ec)
    {
        std::cerr << "[GameModule] copy failed: " << dllPath << " -> " << loadedPath.string()
                  << " (" << ec.message() << ")\n";
        return false;
    }

    lib_ = ::LoadLibraryA(loadedPath.string().c_str());
#else
    lib_ = ::dlopen(dllPath.c_str(), RTLD_NOW);
#endif
    if (!lib_)
    {
        std::cerr << "[GameModule] failed to load " << dllPath << "\n";
        return false;
    }

#if defined(_WIN32)
    // GetProcAddress returns a generic function pointer by design.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
    auto entry = reinterpret_cast<GameModuleEntryFn>(
        ::GetProcAddress(static_cast<HMODULE>(lib_), GameModuleEntryName));
#pragma clang diagnostic pop
#else
    auto entry = reinterpret_cast<GameModuleEntryFn>(::dlsym(lib_, GameModuleEntryName));
#endif
    if (!entry)
    {
        std::cerr << "[GameModule] " << GameModuleEntryName << " not found in " << dllPath << "\n";
        return false;
    }

    entry(&api_);
    ComponentRegistry::instance().importFrom(*api_.registry_);

    // Imported fields point at the DLL's field type slots, which have no
    // editor half.
    for (const ComponentType& t : ComponentRegistry::instance().all())
        for (const Field& f : t.fields)
            if (!f.type->drawUI)
                installFieldUIFor(*f.type);

    ComponentRegistry::instance().validate();
    std::cout << "[GameModule] loaded " << dllPath << "\n";
    return true;
}

std::unique_ptr<Game> GameModuleLoader::makeGame() const
{
    return api_.createGame_ ? std::unique_ptr<Game>(api_.createGame_()) : nullptr;
}

GameModuleLoader::~GameModuleLoader()
{
#if defined(_WIN32)
    if (lib_)
        ::FreeLibrary(static_cast<HMODULE>(lib_));
#else
    if (lib_)
        ::dlclose(lib_);
#endif
}

}  // namespace batap
