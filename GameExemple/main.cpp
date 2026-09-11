#include "GameComponents.h"
#include "MyGame.h"

#include "Engine.h"
#include "Platform/PlatformWindow.h"
#include "World.h"

int main()
{
    batap::Engine engine{{.title = "Batap TestGame",
                          .width = 1280,
                          .height = 720,
                          .fpsInTitle = true,
                          .transparent = true}};

    // `--scene <path>` is how the editor's Run button points at its temp save.
    std::string scenePath = "scenes/Cornel/cornelScene.btpl";
    const auto args = batap::platformCommandLineArgs();
    for (size_t i = 0; i + 1 < args.size(); ++i)
        if (args[i] == "--scene")
            scenePath = args[i + 1];

    batap::World world{engine};
    world.loadScene(scenePath);

    batap::MyGame game;
    game.init(world);

    while (batap::Frame frame = engine.nextFrame())
        world.update(game);

    return 0;
}
