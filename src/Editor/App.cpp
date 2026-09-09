#include "App.h"

#include "Engine.h"
#include "Importers/FileImporter.h"
#include "Platform/PlatformWindow.h"
#include "Serialization/EntitySerializer.h"
#include "TestScene.h"
#include "UI/FieldUI.h"
#include "UI/UITheme.h"
#include "UI/UIPanels.h"
#include "Utils/UIDGenerator.h"
#include "FileDialog.h"

#include <imgui.h>
#include <nlohmann/json.hpp>
#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <memory>

#if defined(_WIN32)
#include <windows.h>
#include <shellapi.h>
#endif

namespace batap
{

App::App(Engine& engine, World& world)
    : ctx_(&engine), world_(&world), assetManager_(ctx_->assetManager_.get())
{
    ui::ApplyTheme();
    installFieldUI();

    world.scene_ = std::make_unique<TestScene>(world);

    loadRecentProjects();
}

void App::update(Frame& frame)
{
    pumpMsgFileDialog();

    if (state_ == AppState::SelectProject)
    {
        uiPanels_.drawStartupScreen(*this, *ctx_);
    }
    else
    {
        uiPanels_.draw(*world_, *this, *ctx_);
        if (playing_ && game_)
            game_->update(*world_, frame);
        world_->update();
    }
}

void App::startPlay()
{
    if (playing_)
        return;
    playSnapshot_ = EntitySerializer::toBuffer(*world_, *ctx_);
    playing_ = true;
    if (game_)
        game_->init(*world_);
}

void App::stopPlay()
{
    if (!playing_)
        return;
    playing_ = false;
    EntitySerializer::clearSceneAndLoadBuffer(*world_, *ctx_, playSnapshot_);
    playSnapshot_.clear();
    // Every EntityHandle from before the reload is dead.
    uiPanels_.clearSelection();
}

static void spawnDetached(const std::string& exe, const std::string& args)
{
#if defined(_WIN32)
    ::ShellExecuteA(nullptr, "open", exe.c_str(), args.c_str(), nullptr, SW_SHOWNORMAL);
#else
    std::system(("\"" + exe + "\" " + args + " &").c_str());
#endif
}

void App::runStandalone()
{
    if (gameExe_.empty())
        return;

    namespace fs = std::filesystem;
    const fs::path scene = fs::temp_directory_path() / "batap_run.btpl";
    EntitySerializer::save(*world_, *ctx_, scene.string());

    fs::path exe = fs::path(platformExeDir()) / gameExe_;
#if defined(_WIN32)
    exe += ".exe";
#endif
    spawnDetached(exe.string(),
                  "--project \"" + projectDir_ + "\" --scene \"" + scene.string() + "\"");
}

// L'emplacement par-utilisateur de chaque OS : %APPDATA% / Application Support
static std::filesystem::path configPath()
{
#if defined(_WIN32)
    char* appdata = nullptr;
    size_t len    = 0;
    _dupenv_s(&appdata, &len, "APPDATA");
    std::filesystem::path base = appdata ? appdata : ".";
    free(appdata);
#else
    const char* home = std::getenv("HOME");
    std::filesystem::path base =
        home ? std::filesystem::path(home) / "Library/Application Support" : ".";
#endif
    return base / "BatapEngine" / "recent.json";
}

void App::loadRecentProjects()
{
    auto path = configPath();
    if (!std::filesystem::exists(path))
        return;
    std::ifstream f(path);
    if (!f.is_open())
        return;
    try
    {
        auto j = nlohmann::json::parse(f);
        for (auto& s : j.value("recent", nlohmann::json::array()))
        {
            auto str = s.get<std::string>();
            if (!str.empty())
                recentProjects_.push_back(std::move(str));
        }
    }
    catch (...) {}
}

void App::saveRecentProjects()
{
    auto path = configPath();
    std::filesystem::create_directories(path.parent_path());
    nlohmann::json j;
    j["recent"] = recentProjects_;
    std::ofstream(path) << j.dump(2);
}

void App::selectProject(const std::string& dir)
{
    projectDir_ = dir;
    ctx_->assetManager_->setBaseDir(dir);
    state_ = AppState::Running;

    recentProjects_.erase(
        std::remove(recentProjects_.begin(), recentProjects_.end(), dir),
        recentProjects_.end());
    recentProjects_.insert(recentProjects_.begin(), dir);
    if (recentProjects_.size() > 10)
        recentProjects_.resize(10);
    saveRecentProjects();
}

uint64_t App::openFileDialogAsyncWithAfterJob(std::span<const FileDialogFilter> filters,
                                              FileDialogAfterJob job)
{
    auto id = next_uid64();
    fileDialogAfterJobs_.emplace(id, std::move(job));
    OpenFilesDialogAsync(filters, &fileDialogMsgBus_, id);
    return id;
}

uint64_t App::openFolderDialogAsyncWithAfterJob(FileDialogAfterJob job)
{
    auto id = next_uid64();
    fileDialogAfterJobs_.emplace(id, std::move(job));
    OpenFolderDialogAsync(&fileDialogMsgBus_, id);
    return id;
}

void App::pumpMsgFileDialog()
{
    fileDialogMsgBus_.pumpType<FileDialogMsg>(
        [&](FileDialogMsg&& msg)
        {
            auto it = fileDialogAfterJobs_.find(msg.id_);
            if (it != fileDialogAfterJobs_.end())
            {
                it->second(std::move(msg.paths_));
                fileDialogAfterJobs_.erase(it);
                return;
            }

            for (auto& path : msg.paths_)
                importFile(path, ImportOptions{projectDir_});
        });
}

}  // namespace batap
