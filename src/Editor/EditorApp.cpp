#include "EditorApp.h"

#include "App.h"
#include "World.h"

#include <exception>
#include <iostream>

namespace batap
{
int runEditor(const EditorConfig& cfg)
{
    try
    {
        Engine engine{cfg.window_};
        World world{engine};
        App app{engine, world};
        if (cfg.makeGame_)
            app.game_ = cfg.makeGame_();
        if (cfg.gameExe_)
            app.gameExe_ = cfg.gameExe_;

        while (Frame frame = engine.nextFrame())
            app.update(frame);

        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << "[FATAL] " << e.what() << "\n";
        return 1;
    }
    catch (...)
    {
        std::cerr << "[FATAL] unknown exception\n";
        return 1;
    }
}
}  // namespace batap
